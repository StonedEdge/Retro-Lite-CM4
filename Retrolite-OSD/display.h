void clearLayer(IMAGE_LAYER_T *layer){
    IMAGE_T *image = &(layer->image);
    clearImageRGB(image, &clearColor);
    changeSourceAndUpdateImageLayer(layer);
}

void displayImage(char address[60], int xPos, int yPos, IMAGE_LAYER_T *imgLayer, int layerOffset){
    if (loadPng(&(imgLayer->image), address) == false) {
        fprintf(stderr, "%s \n", address);
    }
    createResourceImageLayer(imgLayer, layer + layerOffset);
    update = vc_dispmanx_update_start(0);
    assert(update != 0);

    addElementImageLayerOffset(imgLayer, xPos, yPos, display, update);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);
}

void displayText(char text[60], int xPos, int yPos, IMAGE_LAYER_T *imgLayer, int layerOffset, RGBA8_T colour){
    initImageLayer(imgLayer, displayX, displayY, type);
    if(darkMode){
        drawStringRGB(0, 0, text, &colour, &(imgLayer->image));
    } else {
        drawStringRGB(0, 0, text, &whiteColour, &(imgLayer->image));
    }
    
    createResourceImageLayer(imgLayer, layer + layerOffset);
    update = vc_dispmanx_update_start(0);
    assert(update != 0);

    addElementImageLayerOffset(imgLayer, xPos, yPos, display, update);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);
}

void displayLongText(char text[1200], int xPos, int yPos, IMAGE_LAYER_T *imgLayer, int layerOffset, RGBA8_T colour){
    initImageLayer(imgLayer, displayX, displayY, type);
    //if(darkMode){
        drawStringRGB(0, 0, text, &colour, &(imgLayer->image));
    //} else {
        //drawStringRGB(0, 0, text, &whiteColour, &(imgLayer->image));
    //}
    
    createResourceImageLayer(imgLayer, layer + layerOffset);
    update = vc_dispmanx_update_start(0);
    assert(update != 0);

    addElementImageLayerOffset(imgLayer, xPos, yPos, display, update);

    result = vc_dispmanx_update_submit_sync(update);
    assert(result == 0);
}

void updateBatteryText(){
    char temp[5];
    int xOffset = 3;
    strncpy(temp, batteryText, 5);

    //clearImageRGB(&(infoLayer->image), &clearColor);
    snprintf(batteryText, sizeof(batteryText),"%.0f%%", currentSOC);
    if(currentSOC >  99.5){
        xOffset = 4;
    } else if(currentSOC < 9.5){
        xOffset = 2;
    }
    if(strcmp(batteryText,temp) != 0){
        if(batteryPercentLayerEN){
            destroyImageLayer(&batteryPercentLayer);
        }
        displayText(batteryText,  displayX - statusIconSize - (fontWidth * xOffset) -4, 3, &batteryPercentLayer, 1, blackColour);
        batteryPercentLayerEN = true;
    }
}

void displaySetup(){
    checkResolution();
    displayNumber = 0;

    type = VC_IMAGE_RGBA32;

    bcm_host_init();

    display = vc_dispmanx_display_open(displayNumber);
    assert(display != 0);

    result = vc_dispmanx_display_get_info(display, &info);
    assert(result == 0);
    //return result;
}

void batteryMenu(){
    char textBuffer[1200];
    int menuX = (displayX /2) - (overlayImageX / 2);
    int menuY = (displayY /2) - (overlayImageY / 2);
    int textY = menuY + 73;
    int textX = menuX + 88;

    if(!menuLayerEN){
        //Background
        displayImage("./overlays/batteryInfo.png", menuX, menuY, &menuLayer, 1);
        menuLayerEN = true;
    }
    if(max17055_Status != -1){
        //SOC
        sprintf(textBuffer, "Current SOC: %.1f%%\n", currentSOC);

        //Voltage
        sprintf(textBuffer + strlen(textBuffer), "Voltage: %.1fv\n", getInstantaneousVoltage());

        //Current
        sprintf(textBuffer + strlen(textBuffer), "Current: %.2fa\n", getInstantaneousCurrent());

        //Design cap
        sprintf(textBuffer + strlen(textBuffer), "Design Capacity: %dmAh\n", getCapacity());

        //Remaining cap
        sprintf(textBuffer + strlen(textBuffer), "Remaining Capacity: %dmAh\n", getRemainingCapacity());

        //Time to Empty
        sprintf(textBuffer + strlen(textBuffer), "Time to Empty: %.2f hours\n", getTimeToEmpty()); 

        //Charging
        if(charging){
            sprintf(textBuffer + strlen(textBuffer), "Charger Connected\n");
        } else {
            sprintf(textBuffer + strlen(textBuffer), "Charger Disconnected\n");
        }
    } else {
        sprintf(textBuffer, "Max17055 Failed to Connect, \nBattery information unavailable.");
    }
    

    displayLongText(textBuffer, textX, textY, &menuTextLayer, 1, blackColour);
    menuTextLayerEN = true;

    menuDisplayed = 1;
}

void mainMenu(){
    char textBuffer[1200];
    int menuX = (displayX /2) - (overlayImageX / 2);
    int menuY = (displayY /2) - (overlayImageY / 2);
    int textY = menuY + 35;
    int textX = menuX + 61;

    if(!menuLayerEN){
        //Background
        displayImage("./overlays/mainMenu.png", menuX, menuY, &menuLayer, 1);
        menuLayerEN = true;
    }

    //Menu Pointer
    displayImage("./overlays/menuDot.png", textX, textY + (menuSelected * 30), &menuPointerLayer, 1);
    menuPointerLayerEN = true;

    menuDisplayed = 0;
}

void menuOptionSelected(){
    if(menuDisplayed == 0){ //Main Menu
        if(menuSelected == 2){ //Calibrate Controller
            menuActive = 2;
            menuUpdated = true;
            serialPrintf(controllerSerial, "%c", calibrationStepOne);
            calibrationStep = 1;
            calibrationStepChanged = true;
        } else if(menuSelected == 1){ //Battery Info
            menuUpdated = true;
            menuActive = 1;
        } else if(menuSelected == 3){ //On Screen Keyboard
            menuUpdated = true;
            menuActive = 3;
            keyboardChanged = true;
        } else if(menuSelected == 4){ //Settings
            menuUpdated = true;
            menuActive = 4;
            menuSelected = 1;
        }
    } else if(menuDisplayed == 1){ //Battery Info
        menuUpdated = true;
        menuActive = 0;
    } else if(menuDisplayed == 4){//Settings
        if(menuSelected == 7){
            saveValues(); //Save Settings
            serialPrintf(controllerSerial, "%c", menuClose); //Tell controller to exit menu mode
            system("sudo reboot now"); //Reboot
        }
    }

}

void settingValueChanged(int value){
    if(menuSelected == 1){ //Shutdown Voltage
        if(value == 1){
            lowVoltageThreshold += 0.1;
        } else {
            lowVoltageThreshold -= 0.1;
        }
    } else if(menuSelected == 2){ //Battery Capacity
        if(value == 1){
            batteryCapacity += 100;
        } else {
            batteryCapacity -= 100;
        }
    } else if(menuSelected == 3){ //Fan Threshold
        cpuFanThreshold += value;
    } else if(menuSelected == 4){ //Display Select
        if(displaySelect == 1){
            displaySelect = 0;
        } else {
            displaySelect = 1;
        }
    } else if(menuSelected == 5){ //Dark Mode
        if(darkMode == 1){
            darkMode = 0;
        } else {
            darkMode = 1;
        }
    }

    if(menuTextLayerEN){
        destroyImageLayer(&menuTextLayer);
        menuTextLayerEN = false;
    }
    menuUpdated = true;
}

void settingsMenu(){
    char textBuffer[1200];
    int menuX = (displayX /2) - (overlayImageX / 2);
    int menuY = (displayY /2) - (overlayImageY / 2);
    int textY = menuY + 73;
    int textX = menuX + 88;

    if(!menuLayerEN){
        //Background
        displayImage("./overlays/settingsMenu.png", menuX, menuY, &menuLayer, 1);
        menuLayerEN = true;
    }

    //Low Voltage Cutoff
    sprintf(textBuffer, "Shutdown Voltage: %.2fv\n", lowVoltageThreshold);

    //Battery Capacity
    sprintf(textBuffer + strlen(textBuffer), "Battery Capacity: %dmAh \n", batteryCapacity);

    //Fan Temp Threshold
    sprintf(textBuffer + strlen(textBuffer), "Fan Start Temperature: %dc\n", cpuFanThreshold);

    //Display Select
    if(displaySelect == 0){
        sprintf(textBuffer + strlen(textBuffer), "Display: Internal\n");
    } else {
        sprintf(textBuffer + strlen(textBuffer), "Display: External\n");
    }

    //Text Colour
    if(darkMode){  
        sprintf(textBuffer + strlen(textBuffer), "OSD Colour: Black\n");
    } else {
        sprintf(textBuffer + strlen(textBuffer), "OSD Colour: White\n");
    }

    //Save and reboot
    sprintf(textBuffer + strlen(textBuffer), "\nSave and Reboot\n");

    if(!menuTextLayerEN){
        displayLongText(textBuffer, textX, textY, &menuTextLayer, 1, blackColour);
        menuTextLayerEN = true;
    }

    //Menu Pointer
    displayImage("./overlays/menuDot.png", textX - 31, (textY - 26) + (menuSelected * 16), &menuPointerLayer, 1);
    menuPointerLayerEN = true;

    //snprintf(buf, sizeof(buf), "sudo cp /boot/internal.txt /boot/config.txt"); //Enable Internal Display
    //snprintf(buf, sizeof(buf), "sudo cp /boot/external.txt /boot/config.txt"); //Enable External Display
    //system(buf);

    menuDisplayed = 4;
}
