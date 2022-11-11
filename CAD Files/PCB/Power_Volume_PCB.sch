<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.6.1">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="4" name="Route4" color="1" fill="4" visible="no" active="no"/>
<layer number="5" name="Route5" color="4" fill="4" visible="no" active="no"/>
<layer number="6" name="Route6" color="1" fill="8" visible="no" active="no"/>
<layer number="7" name="Route7" color="4" fill="8" visible="no" active="no"/>
<layer number="8" name="Route8" color="1" fill="2" visible="no" active="no"/>
<layer number="9" name="Route9" color="4" fill="2" visible="no" active="no"/>
<layer number="10" name="Route10" color="1" fill="7" visible="no" active="no"/>
<layer number="11" name="Route11" color="4" fill="7" visible="no" active="no"/>
<layer number="12" name="Route12" color="1" fill="5" visible="no" active="no"/>
<layer number="13" name="Route13" color="4" fill="5" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="TL3315NF100Q">
<packages>
<package name="TL3315NF100Q">
<wire x1="-2.25" y1="2.25" x2="2.25" y2="2.25" width="0.127" layer="21"/>
<wire x1="2.25" y1="2.25" x2="2.25" y2="-2.25" width="0.127" layer="21"/>
<wire x1="2.25" y1="-2.25" x2="-2.25" y2="-2.25" width="0.127" layer="21"/>
<wire x1="-2.25" y1="-2.25" x2="-2.25" y2="2.25" width="0.127" layer="21"/>
<text x="0" y="2.50083125" size="1.016340625" layer="25" ratio="12" align="bottom-center">&gt;NAME</text>
<smd name="1" x="-2.225" y="1.75" dx="0.7" dy="0.7" layer="1"/>
<smd name="2" x="2.225" y="1.75" dx="0.7" dy="0.7" layer="1"/>
<smd name="4" x="2.225" y="-1.75" dx="0.7" dy="0.7" layer="1"/>
<smd name="3" x="-2.225" y="-1.75" dx="0.7" dy="0.7" layer="1"/>
</package>
</packages>
<symbols>
<symbol name="PUSH-BUTTON-SWITCH">
<description>Push Button Switch</description>
<wire x1="-7.62" y1="0" x2="-2.54" y2="0" width="0.254" layer="94"/>
<wire x1="2.54" y1="0" x2="7.62" y2="0" width="0.254" layer="94"/>
<wire x1="-2.54" y1="0" x2="1.27" y2="2.54" width="0.254" layer="94"/>
<wire x1="-2.54" y1="3.81" x2="-2.54" y2="5.08" width="0.254" layer="94"/>
<wire x1="-2.54" y1="5.08" x2="0" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="2.54" y2="5.08" width="0.254" layer="94"/>
<wire x1="2.54" y1="5.08" x2="2.54" y2="3.81" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="0" y2="1.016" width="0.254" layer="94" style="shortdash"/>
<circle x="2.54" y="0" radius="0.3592" width="0.254" layer="94"/>
<circle x="-2.54" y="0" radius="0.3592" width="0.254" layer="94"/>
<text x="-2.54425" y="-2.54425" size="1.78096875" layer="95">&gt;NAME</text>
<wire x1="0" y1="2.032" x2="0" y2="3.048" width="0.254" layer="94" style="shortdash"/>
<wire x1="0" y1="4.064" x2="0" y2="5.08" width="0.254" layer="94" style="shortdash"/>
<pin name="P$1" x="-7.62" y="0" visible="pad" length="middle" direction="pas"/>
<pin name="P$2" x="7.62" y="0" visible="pad" length="middle" direction="pas" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="TL3315NF100Q" prefix="S">
<description>SWITCH TACTILE SPST-NO 0.05A 15V </description>
<gates>
<gate name="G$1" symbol="PUSH-BUTTON-SWITCH" x="0" y="0"/>
</gates>
<devices>
<device name="" package="TL3315NF100Q">
<connects>
<connect gate="G$1" pin="P$1" pad="1 2"/>
<connect gate="G$1" pin="P$2" pad="3 4"/>
</connects>
<technologies>
<technology name="">
<attribute name="DESCRIPTION" value=" Tactile Switch SPST-NO Top Actuated Surface Mount "/>
<attribute name="DIGI-KEY_PART_NUMBER" value="EG4620CT-ND"/>
<attribute name="MF" value="E-Switch"/>
<attribute name="MP" value="TL3315NF100Q"/>
<attribute name="PACKAGE" value="None"/>
<attribute name="PURCHASE-URL" value="https://pricing.snapeda.com/search/part/TL3315NF100Q/?ref=eda"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="0781710004">
<packages>
<package name="MOLEX_0781710004">
<text x="-3.4" y="2.2" size="0.8128" layer="25">&gt;NAME</text>
<text x="-3.4" y="2" size="0.8128" layer="27" align="top-left">&gt;VALUE</text>
<wire x1="-3.55" y1="0.68" x2="-3.55" y2="-4.65" width="0.05" layer="39"/>
<wire x1="-3.55" y1="-4.65" x2="3.55" y2="-4.65" width="0.05" layer="39"/>
<wire x1="3.55" y1="-4.65" x2="3.55" y2="0.68" width="0.05" layer="39"/>
<wire x1="3.55" y1="0.68" x2="-3.55" y2="0.68" width="0.05" layer="39"/>
<wire x1="-3.3" y1="-4.495" x2="3.3" y2="-4.495" width="0.127" layer="21"/>
<wire x1="-3.3" y1="0.105" x2="-3.3" y2="-4.395" width="0.127" layer="51"/>
<wire x1="-3.3" y1="-4.395" x2="3.3" y2="-4.395" width="0.127" layer="51"/>
<wire x1="3.3" y1="-4.395" x2="3.3" y2="0.105" width="0.127" layer="51"/>
<wire x1="3.3" y1="0.105" x2="-3.3" y2="0.105" width="0.127" layer="51"/>
<wire x1="-2.5" y1="0.105" x2="-3.3" y2="0.105" width="0.127" layer="21"/>
<wire x1="2.5" y1="0.105" x2="3.3" y2="0.105" width="0.127" layer="21"/>
<wire x1="-3.3" y1="0.105" x2="-3.3" y2="-3" width="0.127" layer="21"/>
<wire x1="3.3" y1="0.105" x2="3.3" y2="-3" width="0.127" layer="21"/>
<circle x="-1.75" y="0.95" radius="0.1" width="0.2" layer="21"/>
<circle x="-1.75" y="0.95" radius="0.1" width="0.2" layer="51"/>
<smd name="4" x="1.8" y="0" dx="0.6" dy="0.85" layer="1"/>
<smd name="3" x="0.6" y="0" dx="0.6" dy="0.85" layer="1"/>
<smd name="2" x="-0.6" y="0" dx="0.6" dy="0.85" layer="1"/>
<smd name="S1" x="2.95" y="-3.775" dx="0.7" dy="0.8" layer="1"/>
<smd name="S2" x="-2.95" y="-3.775" dx="0.7" dy="0.8" layer="1"/>
<smd name="1" x="-1.8" y="0" dx="0.6" dy="0.85" layer="1"/>
</package>
</packages>
<symbols>
<symbol name="0781710004">
<wire x1="0" y1="6.35" x2="1.27" y2="7.62" width="0.254" layer="94"/>
<wire x1="0" y1="6.35" x2="0" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="-8.89" width="0.254" layer="94"/>
<wire x1="0" y1="-8.89" x2="1.27" y2="-10.16" width="0.254" layer="94"/>
<wire x1="1.27" y1="-10.16" x2="12.7" y2="-10.16" width="0.254" layer="94"/>
<wire x1="12.7" y1="-10.16" x2="12.7" y2="7.62" width="0.254" layer="94"/>
<wire x1="12.7" y1="7.62" x2="1.27" y2="7.62" width="0.254" layer="94"/>
<wire x1="0" y1="5.08" x2="1.905" y2="5.08" width="0.254" layer="94"/>
<wire x1="0" y1="2.54" x2="1.905" y2="2.54" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="-2.54" x2="1.905" y2="-2.54" width="0.254" layer="94"/>
<wire x1="0" y1="-7.62" x2="1.905" y2="-7.62" width="0.254" layer="94"/>
<text x="0" y="8.89" size="1.778" layer="95">&gt;NAME</text>
<text x="0" y="-12.7" size="1.778" layer="96">&gt;VALUE</text>
<rectangle x1="0.635" y1="4.7625" x2="2.2225" y2="5.3975" layer="94"/>
<rectangle x1="0.635" y1="2.2225" x2="2.2225" y2="2.8575" layer="94"/>
<rectangle x1="0.635" y1="-0.3175" x2="2.2225" y2="0.3175" layer="94"/>
<rectangle x1="0.635" y1="-2.8575" x2="2.2225" y2="-2.2225" layer="94"/>
<rectangle x1="0.635" y1="-7.9375" x2="2.2225" y2="-7.3025" layer="94"/>
<pin name="1" x="-5.08" y="5.08" length="middle" direction="pas"/>
<pin name="2" x="-5.08" y="2.54" length="middle" direction="pas"/>
<pin name="3" x="-5.08" y="0" length="middle" direction="pas"/>
<pin name="4" x="-5.08" y="-2.54" length="middle" direction="pas"/>
<pin name="SHIELD" x="-5.08" y="-7.62" length="middle" direction="pas"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="0781710004" prefix="J">
<description>Connector Header Surface Mount 4 position 0.047in (1.20mm)  &lt;a href="https://pricing.snapeda.com/parts/0781710004/Molex/view-part?ref=eda"&gt;Check availability&lt;/a&gt;</description>
<gates>
<gate name="G$1" symbol="0781710004" x="0" y="0"/>
</gates>
<devices>
<device name="" package="MOLEX_0781710004">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="SHIELD" pad="S1 S2"/>
</connects>
<technologies>
<technology name="">
<attribute name="AVAILABILITY" value="In Stock"/>
<attribute name="DESCRIPTION" value=" Connector Header Surface Mount 4 position 0.047 (1.20mm) "/>
<attribute name="MF" value="Molex"/>
<attribute name="MP" value="0781710004"/>
<attribute name="PACKAGE" value="None"/>
<attribute name="PRICE" value="None"/>
<attribute name="PURCHASE-URL" value="https://pricing.snapeda.com/search/part/0781710004/?ref=eda"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="V--" library="TL3315NF100Q" deviceset="TL3315NF100Q" device=""/>
<part name="V++" library="TL3315NF100Q" deviceset="TL3315NF100Q" device=""/>
<part name="PWR_BTN" library="TL3315NF100Q" deviceset="TL3315NF100Q" device=""/>
<part name="J1" library="0781710004" deviceset="0781710004" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="V--" gate="G$1" x="33.02" y="48.26" smashed="yes">
<attribute name="NAME" x="30.47575" y="45.71575" size="1.78096875" layer="95"/>
</instance>
<instance part="V++" gate="G$1" x="53.34" y="48.26" smashed="yes">
<attribute name="NAME" x="50.79575" y="45.71575" size="1.78096875" layer="95"/>
</instance>
<instance part="PWR_BTN" gate="G$1" x="96.52" y="48.26" smashed="yes">
<attribute name="NAME" x="93.97575" y="45.71575" size="1.78096875" layer="95"/>
</instance>
<instance part="J1" gate="G$1" x="-12.7" y="48.26" smashed="yes" rot="R180">
<attribute name="NAME" x="-12.7" y="39.37" size="1.778" layer="95" rot="R180"/>
<attribute name="VALUE" x="-12.7" y="60.96" size="1.778" layer="96" rot="R180"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="GND" class="0">
<segment>
<wire x1="7.62" y1="53.34" x2="7.62" y2="55.88" width="0.1524" layer="91"/>
<pinref part="PWR_BTN" gate="G$1" pin="P$2"/>
<wire x1="7.62" y1="55.88" x2="40.64" y2="55.88" width="0.1524" layer="91"/>
<wire x1="40.64" y1="55.88" x2="60.96" y2="55.88" width="0.1524" layer="91"/>
<wire x1="60.96" y1="55.88" x2="104.14" y2="55.88" width="0.1524" layer="91"/>
<wire x1="104.14" y1="55.88" x2="104.14" y2="48.26" width="0.1524" layer="91"/>
<pinref part="V++" gate="G$1" pin="P$2"/>
<wire x1="60.96" y1="48.26" x2="60.96" y2="55.88" width="0.1524" layer="91"/>
<junction x="60.96" y="55.88"/>
<pinref part="V--" gate="G$1" pin="P$2"/>
<wire x1="40.64" y1="48.26" x2="40.64" y2="55.88" width="0.1524" layer="91"/>
<junction x="40.64" y="55.88"/>
<label x="15.24" y="55.88" size="1.778" layer="95" rot="R90" xref="yes"/>
<wire x1="0" y1="53.34" x2="7.62" y2="53.34" width="0.1524" layer="91"/>
<pinref part="J1" gate="G$1" pin="SHIELD"/>
<wire x1="-7.62" y1="55.88" x2="0" y2="55.88" width="0.1524" layer="91"/>
<wire x1="0" y1="55.88" x2="0" y2="53.34" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="J1" gate="G$1" pin="1"/>
<wire x1="-7.62" y1="43.18" x2="-7.62" y2="40.64" width="0.1524" layer="91"/>
<label x="-7.62" y="35.56" size="1.778" layer="95" rot="R90"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="V++" gate="G$1" pin="P$1"/>
<wire x1="-2.54" y1="48.26" x2="-2.54" y2="40.64" width="0.1524" layer="91"/>
<wire x1="-2.54" y1="40.64" x2="45.72" y2="40.64" width="0.1524" layer="91"/>
<wire x1="45.72" y1="40.64" x2="45.72" y2="48.26" width="0.1524" layer="91"/>
<pinref part="J1" gate="G$1" pin="3"/>
<wire x1="-7.62" y1="48.26" x2="-2.54" y2="48.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="J1" gate="G$1" pin="4"/>
<pinref part="PWR_BTN" gate="G$1" pin="P$1"/>
<wire x1="7.62" y1="30.48" x2="88.9" y2="30.48" width="0.1524" layer="91"/>
<wire x1="88.9" y1="30.48" x2="88.9" y2="48.26" width="0.1524" layer="91"/>
<wire x1="-7.62" y1="50.8" x2="7.62" y2="50.8" width="0.1524" layer="91"/>
<wire x1="7.62" y1="50.8" x2="7.62" y2="30.48" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="J1" gate="G$1" pin="2"/>
<pinref part="V--" gate="G$1" pin="P$1"/>
<wire x1="-7.62" y1="45.72" x2="25.4" y2="45.72" width="0.1524" layer="91"/>
<wire x1="25.4" y1="45.72" x2="25.4" y2="48.26" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
