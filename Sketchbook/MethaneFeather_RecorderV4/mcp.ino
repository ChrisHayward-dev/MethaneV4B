// MCP check routines
//#include <MCP342x.h>
//
//uint8_t mcpAddress = 0x6f;  // 6e is the address w/ jumper open
//MCP342x adc = MCP342x(mcpAddress);
//
//
//
//float mcpLSB = (2.0 * 2.048) / pow(2.0, 18);
//
//long mcp_value[5]; //we use 5 here so we can number by channel starting at 1
//
//void mcp_printGas(Stream *device) {
//  float Vsupply  = mcpLSB * 2*mcp_value[3];;                // voltage at bridge supply
//  float Vref     = mcpLSB * mcp_value[3];                   // voltage on reference half bridge
//  float Vdiff    = mcpLSB * mcp_value[4];
//  float Vgas     = Vref + Vdiff;
//  float Rgas     = Vgas * (R_REF/(Vsupply - Vgas));
//  device->print(Rgas);
//  device->print(",");
////  Serial.print("Vsupply:");Serial.print(Vsupply,5);
////  Serial.print(", Vref:");Serial.print(Vref.5);
////  Serial.print(", Vdiff:");Serial.print(Vdiff,5);
////  Serial.print(", Vgas:");Serial.print(Vgas,5);
////  Serial.print(", Rgas:");Serial.println(Rgas);
//  Serial.println(Rgas);
//}
//void mcp_printVals(Stream *device) {
//  for(int k=1;k<5;k++) {
//    device->print(mcp_value[k]*mcpLSB,6);
//    device->print(",");
//  }
//}
//void mcp_process() {
//  mcp_readChan(1);
//  mcp_readChan(2);
//  mcp_readChan(3);
//  mcp_readChan(4);  
//}
//
//bool  mcp_readChan(int chan) {
//  MCP342x::Config status;
//  bool err;
//  switch (chan) {
//    case 1:
//      err = adc.convertAndRead(MCP342x::channel1, MCP342x::oneShot,
//                               MCP342x::resolution18, MCP342x::gain1,
//                               1000000, mcp_value[1], status);
//      break;
//    case 2:
//      err = adc.convertAndRead(MCP342x::channel2, MCP342x::oneShot,
//                               MCP342x::resolution18, MCP342x::gain1,
//                               1000000, mcp_value[2], status);
//      break;
//    case 3:
//      err = adc.convertAndRead(MCP342x::channel3, MCP342x::oneShot,
//                               MCP342x::resolution18, MCP342x::gain1,
//                               1000000, mcp_value[3], status);
//      break;
//    case 4:
//      err = adc.convertAndRead(MCP342x::channel4, MCP342x::oneShot,
//                               MCP342x::resolution18, MCP342x::gain1,
//                               1000000, mcp_value[4], status);
//      break;
//    default:
//      Serial.println("Unknown Channel!");
//      return (false);
//  }
//  if (err) {
//    Serial.println(">>>Convert Error");
//    return(false);
//  }
//  return(true);
//}
//
//void mcp_setup() {
//  MCP342x::Config status;
//  MCP342x::generalCallReset();
//  delay(1); // MC342x needs 300us to settle, wait 1ms
//  Wire.begin();
//  // Check device present
//  Wire.requestFrom(mcpAddress, (uint8_t)1);
//  while (!Wire.available()) {
//    Serial.print("No device found at address ");
//    Serial.println(mcpAddress, HEX);
//    delay(2000);
//  }
//  Serial.println("Identified MCP3424"); //FIXME: Need to do whoami for ident.
//  Serial.print("Using LSB of ");
//  Serial.print(mcpLSB, 6);
//  Serial.println(" volts/bit");
//
//  // check MCP A2D channels in order
//  mcpReadChannel(1,"External TGS2611");
//  mcpReadChannel(2,"5 volt monitor");
//  mcpReadChannel(3,"Bridge Reference");
//  mcpReadChannel(4,"Internal TGS2611");
//  return;
//}
//
//bool mcpReadChannel(uint8_t chanNum, const char *chanName) {
//  bool err;
//  MCP342x::Config status;
//  mcp_readChan(chanNum);
//
//  Serial.print("MCP3424 channel ");
//  Serial.print(chanNum);
//  Serial.print(" - ");
//  Serial.print(chanName);
//  Serial.print(":");
//  Serial.print(mcp_value[chanNum]);
//  Serial.print(" (");
//  Serial.print(mcp_value[chanNum] * mcpLSB, 6);
//  Serial.println(" volts)");
//  return(true);
//}
