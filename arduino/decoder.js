function Decoder(b, port) {
    // LSB, Least Significant Bit/Byte first!
    var decoded = {};
    //https://www.thethingsnetwork.org/forum/t/best-practices-when-sending-gps-location-data-howto/1242
    // Sign-extend the 3rd and 6th bytes into a 4th and 8th byte:
    decoded.latitude = ((b[0] | b[1]<<8 | b[2]<<16 | (b[2] & 0x80 ? 0xFF<<24 : 0)) / 100000).toString();
    decoded.longitude = ((b[3] | b[4]<<8 | b[5]<<16 | (b[5] & 0x80 ? 0xFF<<24 : 0)) / 100000).toString();
  
    return decoded;
    
  }