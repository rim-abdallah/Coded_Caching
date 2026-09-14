#include "Conversions.h"



namespace caching{

//CONVERSION: 8 BPSK symbos ---> INT
void conv_8BPSKsymb_to_int(gr_complex *buff_bpsk, unsigned int &x)
{    
    x=0;
    for(int i=0; i<8; i++){
        x += (real(buff_bpsk[i]) > 0 ? 1 : 0)*pow(2,i);
    }

}

//CONVERSION: 8 BPSK symbos ---> CHAR
void conv_8BPSKsymb_to_char(gr_complex *buff_bpsk, char &x)
{    
    unsigned int s;
    conv_8BPSKsymb_to_int(buff_bpsk, s);
    x = (char) s;

}


//CONVERSION: 16 BPSK symbos ---> INT
void conv_16BPSKsymb_to_int(gr_complex *buff_bpsk, unsigned int &x)
{    
    gr_complex buff_bpsk_c[16];
    x=0;

    //Preliminary process because of convert to short int 
    //swap the least and most significant bytes
    for(int i=0; i<8; i++)
        buff_bpsk_c[i] = buff_bpsk[i+8];
    for(int i=0; i<8; i++)
        buff_bpsk_c[i+8] = buff_bpsk[i];
    

    for(int i=0; i<16; i++){
        //s[i] = real(buff_bpsk[i])>0?1:0;
        x += (real(buff_bpsk_c[i]) > 0 ? 1 : 0)*pow(2,i);
    }

}



//CONVERSION: 4 QPSK symbos ---> INT
void conv_4QPSKsymb_to_int(gr_complex *buff_qpsk, unsigned int &x)
{    
    unsigned int s[4];
    x=0;
    for(int i=0; i<4; i++){
        s[i] = 2*(imag(buff_qpsk[i])>0) + (real(buff_qpsk[i])>0);
        x += s[i]*pow(2,2*i);
    }

}

//CONVERSION: 4 QPSK symbos ---> CHAR
void conv_4QPSKsymb_to_char(gr_complex *buff_qpsk, char &x)
{    
    unsigned int s;
    conv_4QPSKsymb_to_int(buff_qpsk, s);
    x = (char) s;

}



//CONVERSION: 8 QPSK symbos ---> INT
void conv_8QPSKsymb_to_int(gr_complex *buff_qpsk, unsigned int &x)
{    
    gr_complex buff_qpsk_c[8];
    unsigned int s[8];
    //Preliminary process becasue convert to short int 
    //swap the least and most significant bytes
    for(int i=0; i<4; i++)
        buff_qpsk_c[i] = buff_qpsk[i+4];
    for(int i=0; i<4; i++)
        buff_qpsk_c[i+4] = buff_qpsk[i];

    //Main conversion process
    x=0;
    for(int i=0; i<8; i++){
        s[i] = 2*(imag(buff_qpsk_c[i])>0) + (real(buff_qpsk_c[i])>0);
        x += s[i]*pow(2,2*i);
        //cout << s[i] << ", ";
    }
    //cout << endl;
}

//CONVERSION: CHAR ---> BIT
std::vector<unsigned int> conv_char_to_bitsInt(unsigned char c)
{

    std::vector<unsigned int> bytes (8,0);
    for(int i=0; i<8; i++){
        bytes[i] = (c >> i) & 1;
    }
    return bytes;

}

//CONVERSION: INT ---> CHAR
void conv_int_to_char(unsigned int n, char* bytes)
{

    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;

}
//CONVERSION: CHAR ---> INT
void conv_char_to_int(char* bytes, unsigned int &x)
{   
    char change[4];

    change[0]=bytes[3];
    change[1]=bytes[2];
    change[2]=bytes[1];
    change[3]=bytes[0];

    x = *((unsigned int*) change);
}



void conv_short_int_to_char(unsigned short int n, char* bytes)
{

    bytes[0] = (n >> 8) & 0xFF;
    bytes[1] = n & 0xFF;

}

void conv_char_to_short_int(char* bytes, unsigned short int &x)
{
    char change[2];

    change[0]=bytes[1];
    change[1]=bytes[0];

    x = *((unsigned short int*) change);
}

//Convert a stream of chars to stream of bits
std::vector<unsigned int> conv_stream_char_to_bits(const char *in, int packetSize){

    std::vector<unsigned int> bb (8,0);
    std::vector<unsigned int> data_bits;

    for(int j=0; j<packetSize; j++){
        bb = conv_char_to_bitsInt(in[j]);
        for(int k=0; k<8; k++)
            data_bits.push_back(bb[k]);

    }
    return data_bits;

}

void conv_int_to_byte(unsigned int integer, char &b)
{
    unsigned char byte;

    switch(integer)
    {
        // 0 - 15


        case(0):
            byte=0x00;
            break;

        case(1):
            byte=0x01;
            break;

        case(2):
            byte=0x02;
            break;

        case(3):
            byte=0x03;
            break;

        case(4):
            byte=0x04;
            break;

        case(5):
            byte=0x05;
            break;

        case(6):
            byte=0x06;
            break;

        case(7):
            byte=0x07;
            break;

        case(8):
            byte=0x08;
            break;

        case(9):
            byte=0x09;
            break;

        case(10):
            byte=0x0A;
            break;

        case(11):
            byte=0x0B;
            break;

        case(12):
            byte=0x0C;
            break;

        case(13):
            byte=0x0D;
            break;

        case(14):
            byte=0x0E;
            break;

        case(15):
            byte=0x0F;
            break;


        // 16 - 31


        case(16):
            byte=0x10;
            break;

        case(17):
            byte=0x11;
            break;

        case(18):
            byte=0x12;
            break;

        case(19):
            byte=0x13;
            break;

        case(20):
            byte=0x14;
            break;

        case(21):
            byte=0x15;
            break;

        case(22):
            byte=0x16;
            break;

        case(23):
            byte=0x17;
            break;

        case(24):
            byte=0x18;
            break;

        case(25):
            byte=0x19;
            break;

        case(26):
            byte=0x1A;
            break;

        case(27):
            byte=0x1B;
            break;

        case(28):
            byte=0x1C;
            break;

        case(29):
            byte=0x1D;
            break;

        case(30):
            byte=0x1E;
            break;

        case(31):
            byte=0x1F;
            break;


        // 32 - 47


        case(32):
            byte=0x20;
            break;

        case(33):
            byte=0x21;
            break;

        case(34):
            byte=0x22;
            break;

        case(35):
            byte=0x23;
            break;

        case(36):
            byte=0x24;
            break;

        case(37):
            byte=0x25;
            break;

        case(38):
            byte=0x26;
            break;

        case(39):
            byte=0x27;
            break;

        case(40):
            byte=0x28;
            break;

        case(41):
            byte=0x29;
            break;

        case(42):
            byte=0x2A;
            break;

        case(43):
            byte=0x2B;
            break;

        case(44):
            byte=0x2C;
            break;

        case(45):
            byte=0x2D;
            break;

        case(46):
            byte=0x2E;
            break;

        case(47):
            byte=0x2F;
            break;


        // 48 - 63


        case(48):
            byte=0x30;
            break;

        case(49):
            byte=0x31;
            break;

        case(50):
            byte=0x32;
            break;

        case(51):
            byte=0x33;
            break;

        case(52):
            byte=0x34;
            break;

        case(53):
            byte=0x35;
            break;

        case(54):
            byte=0x36;
            break;

        case(55):
            byte=0x37;
            break;

        case(56):
            byte=0x38;
            break;

        case(57):
            byte=0x39;
            break;

        case(58):
            byte=0x3A;
            break;

        case(59):
            byte=0x3B;
            break;

        case(60):
            byte=0x3C;
            break;

        case(61):
            byte=0x3D;
            break;

        case(62):
            byte=0x3E;
            break;

        case(63):
            byte=0x3F;
            break;


        // 64 - 79


        case(64):
            byte=0x40;
            break;

        case(65):
            byte=0x41;
            break;

        case(66):
            byte=0x42;
            break;

        case(67):
            byte=0x43;
            break;

        case(68):
            byte=0x44;
            break;

        case(69):
            byte=0x45;
            break;

        case(70):
            byte=0x46;
            break;

        case(71):
            byte=0x47;
            break;

        case(72):
            byte=0x48;
            break;

        case(73):
            byte=0x49;
            break;

        case(74):
            byte=0x4A;
            break;

        case(75):
            byte=0x4B;
            break;

        case(76):
            byte=0x4C;
            break;

        case(77):
            byte=0x4D;
            break;

        case(78):
            byte=0x4E;
            break;

        case(79):
            byte=0x4F;
            break;


        // 80 - 95


        case(80):
            byte=0x50;
            break;

        case(81):
            byte=0x51;
            break;

        case(82):
            byte=0x52;
            break;

        case(83):
            byte=0x53;
            break;

        case(84):
            byte=0x54;
            break;

        case(85):
            byte=0x55;
            break;

        case(86):
            byte=0x56;
            break;

        case(87):
            byte=0x57;
            break;

        case(88):
            byte=0x58;
            break;

        case(89):
            byte=0x59;
            break;

        case(90):
            byte=0x5A;
            break;

        case(91):
            byte=0x5B;
            break;

        case(92):
            byte=0x5C;
            break;

        case(93):
            byte=0x5D;
            break;

        case(94):
            byte=0x5E;
            break;

        case(95):
            byte=0x5F;
            break;


        // 96 - 111


        case(96):
            byte=0x60;
            break;

        case(97):
            byte=0x61;
            break;

        case(98):
            byte=0x62;
            break;

        case(99):
            byte=0x63;
            break;

        case(100):
            byte=0x64;
            break;

        case(101):
            byte=0x65;
            break;

        case(102):
            byte=0x66;
            break;

        case(103):
            byte=0x67;
            break;

        case(104):
            byte=0x68;
            break;

        case(105):
            byte=0x69;
            break;

        case(106):
            byte=0x6A;
            break;

        case(107):
            byte=0x6B;
            break;

        case(108):
            byte=0x6C;
            break;

        case(109):
            byte=0x6D;
            break;

        case(110):
            byte=0x6E;
            break;

        case(111):
            byte=0x6F;
            break;


        // 112 - 127


        case(112):
            byte=0x70;
            break;

        case(113):
            byte=0x71;
            break;

        case(114):
            byte=0x72;
            break;

        case(115):
            byte=0x73;
            break;

        case(116):
            byte=0x74;
            break;

        case(117):
            byte=0x75;
            break;

        case(118):
            byte=0x76;
            break;

        case(119):
            byte=0x77;
            break;

        case(120):
            byte=0x78;
            break;

        case(121):
            byte=0x79;
            break;

        case(122):
            byte=0x7A;
            break;

        case(123):
            byte=0x7B;
            break;

        case(124):
            byte=0x7C;
            break;

        case(125):
            byte=0x7D;
            break;

        case(126):
            byte=0x7E;
            break;

        case(127):
            byte=0x7F;
            break;


        // 128 - 143


        case(128):
            byte=0x80;
            break;

        case(129):
            byte=0x81;
            break;

        case(130):
            byte=0x82;
            break;

        case(131):
            byte=0x83;
            break;

        case(132):
            byte=0x84;
            break;

        case(133):
            byte=0x85;
            break;

        case(134):
            byte=0x86;
            break;

        case(135):
            byte=0x87;
            break;

        case(136):
            byte=0x88;
            break;

        case(137):
            byte=0x89;
            break;

        case(138):
            byte=0x8A;
            break;

        case(139):
            byte=0x8B;
            break;

        case(140):
            byte=0x8C;
            break;

        case(141):
            byte=0x8D;
            break;

        case(142):
            byte=0x8E;
            break;

        case(143):
            byte=0x8F;
            break;


        // 144 - 159


        case(144):
            byte=0x90;
            break;

        case(145):
            byte=0x91;
            break;

        case(146):
            byte=0x92;
            break;

        case(147):
            byte=0x93;
            break;

        case(148):
            byte=0x94;
            break;

        case(149):
            byte=0x95;
            break;

        case(150):
            byte=0x96;
            break;

        case(151):
            byte=0x97;
            break;

        case(152):
            byte=0x98;
            break;

        case(153):
            byte=0x99;
            break;

        case(154):
            byte=0x9A;
            break;

        case(155):
            byte=0x9B;
            break;

        case(156):
            byte=0x9C;
            break;

        case(157):
            byte=0x9D;
            break;

        case(158):
            byte=0x9E;
            break;

        case(159):
            byte=0x9F;
            break;


        // 160 - 175


        case(160):
            byte=0xA0;
            break;

        case(161):
            byte=0xA1;
            break;

        case(162):
            byte=0xA2;
            break;

        case(163):
            byte=0xA3;
            break;

        case(164):
            byte=0xA4;
            break;

        case(165):
            byte=0xA5;
            break;

        case(166):
            byte=0xA6;
            break;

        case(167):
            byte=0xA7;
            break;

        case(168):
            byte=0xA8;
            break;

        case(169):
            byte=0xA9;
            break;

        case(170):
            byte=0xAA;
            break;

        case(171):
            byte=0xAB;
            break;

        case(172):
            byte=0xAC;
            break;

        case(173):
            byte=0xAD;
            break;

        case(174):
            byte=0xAE;
            break;

        case(175):
            byte=0xAF;
            break;


        // 176 - 191


        case(176):
            byte=0xB0;
            break;

        case(177):
            byte=0xB1;
            break;

        case(178):
            byte=0xB2;
            break;

        case(179):
            byte=0xB3;
            break;

        case(180):
            byte=0xB4;
            break;

        case(181):
            byte=0xB5;
            break;

        case(182):
            byte=0xB6;
            break;

        case(183):
            byte=0xB7;
            break;

        case(184):
            byte=0xB8;
            break;

        case(185):
            byte=0xB9;
            break;

        case(186):
            byte=0xBA;
            break;

        case(187):
            byte=0xBB;
            break;

        case(188):
            byte=0xBC;
            break;

        case(189):
            byte=0xBD;
            break;

        case(190):
            byte=0xBE;
            break;

        case(191):
            byte=0xBF;
            break;


        // 192 - 207


        case(192):
            byte=0xC0;
            break;

        case(193):
            byte=0xC1;
            break;

        case(194):
            byte=0xC2;
            break;

        case(195):
            byte=0xC3;
            break;

        case(196):
            byte=0xC4;
            break;

        case(197):
            byte=0xC5;
            break;

        case(198):
            byte=0xC6;
            break;

        case(199):
            byte=0xC7;
            break;

        case(200):
            byte=0xC8;
            break;

        case(201):
            byte=0xC9;
            break;

        case(202):
            byte=0xCA;
            break;

        case(203):
            byte=0xCB;
            break;

        case(204):
            byte=0xCC;
            break;

        case(205):
            byte=0xCD;
            break;

        case(206):
            byte=0xCE;
            break;

        case(207):
            byte=0xCF;
            break;


        // 208 - 223


        case(208):
            byte=0xD0;
            break;

        case(209):
            byte=0xD1;
            break;

        case(210):
            byte=0xD2;
            break;

        case(211):
            byte=0xD3;
            break;

        case(212):
            byte=0xD4;
            break;

        case(213):
            byte=0xD5;
            break;

        case(214):
            byte=0xD6;
            break;

        case(215):
            byte=0xD7;
            break;

        case(216):
            byte=0xD8;
            break;

        case(217):
            byte=0xD9;
            break;

        case(218):
            byte=0xDA;
            break;

        case(219):
            byte=0xDB;
            break;

        case(220):
            byte=0xDC;
            break;

        case(221):
            byte=0xDD;
            break;

        case(222):
            byte=0xDE;
            break;

        case(223):
            byte=0xDF;
            break;


        // 224 - 239


        case(224):
            byte=0xE0;
            break;

        case(225):
            byte=0xE1;
            break;

        case(226):
            byte=0xE2;
            break;

        case(227):
            byte=0xE3;
            break;

        case(228):
            byte=0xE4;
            break;

        case(229):
            byte=0xE5;
            break;

        case(230):
            byte=0xE6;
            break;

        case(231):
            byte=0xE7;
            break;

        case(232):
            byte=0xE8;
            break;

        case(233):
            byte=0xE9;
            break;

        case(234):
            byte=0xEA;
            break;

        case(235):
            byte=0xEB;
            break;

        case(236):
            byte=0xEC;
            break;

        case(237):
            byte=0xED;
            break;

        case(238):
            byte=0xEE;
            break;

        case(239):
            byte=0xEF;
            break;


        // 240 - 255


        case(240):
            byte=0xF0;
            break;

        case(241):
            byte=0xF1;
            break;

        case(242):
            byte=0xF2;
            break;

        case(243):
            byte=0xF3;
            break;

        case(244):
            byte=0xF4;
            break;

        case(245):
            byte=0xF5;
            break;

        case(246):
            byte=0xF6;
            break;

        case(247):
            byte=0xF7;
            break;

        case(248):
            byte=0xF8;
            break;

        case(249):
            byte=0xF9;
            break;

        case(250):
            byte=0xFA;
            break;

        case(251):
            byte=0xFB;
            break;

        case(252):
            byte=0xFC;
            break;

        case(253):
            byte=0xFD;
            break;

        case(254):
            byte=0xFE;
            break;

        case(255):
            byte=0xFF;
            break;
    }

    b = byte;
}


void conv_byte_to_int(char b, unsigned int &integer)
{
    unsigned char byte = b;

    switch(byte)
    {
        // 0 - 15


        case(0x00):
            integer=0;
            break;

        case(0x01):
            integer=1;
            break;

        case(0x02):
            integer=2;
            break;

        case(0x03):
            integer=3;
            break;

        case(0x04):
            integer=4;
            break;

        case(0x05):
            integer=5;
            break;

        case(0x06):
            integer=6;
            break;

        case(0x07):
            integer=7;
            break;

        case(0x08):
            integer=8;
            break;

        case(0x09):
            integer=9;
            break;

        case(0x0A):
            integer=10;
            break;

        case(0x0B):
            integer=11;
            break;

        case(0x0C):
            integer=12;
            break;

        case(0x0D):
            integer=13;
            break;

        case(0x0E):
            integer=14;
            break;

        case(0x0F):
            integer=15;
            break;


        // 16 - 31


        case(0x10):
            integer=16;
            break;

        case(0x11):
            integer=17;
            break;

        case(0x12):
            integer=18;
            break;

        case(0x13):
            integer=19;
            break;

        case(0x14):
            integer=20;
            break;

        case(0x15):
            integer=21;
            break;

        case(0x16):
            integer=22;
            break;

        case(0x17):
            integer=23;
            break;

        case(0x18):
            integer=24;
            break;

        case(0x19):
            integer=25;
            break;

        case(0x1A):
            integer=26;
            break;

        case(0x1B):
            integer=27;
            break;

        case(0x1C):
            integer=28;
            break;

        case(0x1D):
            integer=29;
            break;

        case(0x1E):
            integer=30;
            break;

        case(0x1F):
            integer=31;
            break;


        // 32 - 47


        case(0x20):
            integer=32;
            break;

        case(0x21):
            integer=33;
            break;

        case(0x22):
            integer=34;
            break;

        case(0x23):
            integer=35;
            break;

        case(0x24):
            integer=36;
            break;

        case(0x25):
            integer=37;
            break;

        case(0x26):
            integer=38;
            break;

        case(0x27):
            integer=39;
            break;

        case(0x28):
            integer=40;
            break;

        case(0x29):
            integer=41;
            break;

        case(0x2A):
            integer=42;
            break;

        case(0x2B):
            integer=43;
            break;

        case(0x2C):
            integer=44;
            break;

        case(0x2D):
            integer=45;
            break;

        case(0x2E):
            integer=46;
            break;

        case(0x2F):
            integer=47;
            break;


        // 48 - 63


        case(0x30):
            integer=48;
            break;

        case(0x31):
            integer=49;
            break;

        case(0x32):
            integer=50;
            break;

        case(0x33):
            integer=51;
            break;

        case(0x34):
            integer=52;
            break;

        case(0x35):
            integer=53;
            break;

        case(0x36):
            integer=54;
            break;

        case(0x37):
            integer=55;
            break;

        case(0x38):
            integer=56;
            break;

        case(0x39):
            integer=57;
            break;

        case(0x3A):
            integer=58;
            break;

        case(0x3B):
            integer=59;
            break;

        case(0x3C):
            integer=60;
            break;

        case(0x3D):
            integer=61;
            break;

        case(0x3E):
            integer=62;
            break;

        case(0x3F):
            integer=63;
            break;


        // 64 - 79


        case(0x40):
            integer=64;
            break;

        case(0x41):
            integer=65;
            break;

        case(0x42):
            integer=66;
            break;

        case(0x43):
            integer=67;
            break;

        case(0x44):
            integer=68;
            break;

        case(0x45):
            integer=69;
            break;

        case(0x46):
            integer=70;
            break;

        case(0x47):
            integer=71;
            break;

        case(0x48):
            integer=72;
            break;

        case(0x49):
            integer=73;
            break;

        case(0x4A):
            integer=74;
            break;

        case(0x4B):
            integer=75;
            break;

        case(0x4C):
            integer=76;
            break;

        case(0x4D):
            integer=77;
            break;

        case(0x4E):
            integer=78;
            break;

        case(0x4F):
            integer=79;
            break;


        // 80 - 95


        case(0x50):
            integer=80;
            break;

        case(0x51):
            integer=81;
            break;

        case(0x52):
            integer=82;
            break;

        case(0x53):
            integer=83;
            break;

        case(0x54):
            integer=84;
            break;

        case(0x55):
            integer=85;
            break;

        case(0x56):
            integer=86;
            break;

        case(0x57):
            integer=87;
            break;

        case(0x58):
            integer=88;
            break;

        case(0x59):
            integer=89;
            break;

        case(0x5A):
            integer=90;
            break;

        case(0x5B):
            integer=91;
            break;

        case(0x5C):
            integer=92;
            break;

        case(0x5D):
            integer=93;
            break;

        case(0x5E):
            integer=94;
            break;

        case(0x5F):
            integer=95;
            break;


        // 96 - 111

        case(0x60):
            integer=96;
            break;

        case(0x61):
            integer=97;
            break;

        case(0x62):
            integer=98;
            break;

        case(0x63):
            integer=99;
            break;

        case(0x64):
            integer=100;
            break;

        case(0x65):
            integer=101;
            break;

        case(0x66):
            integer=102;
            break;

        case(0x67):
            integer=103;
            break;

        case(0x68):
            integer=104;
            break;

        case(0x69):
            integer=105;
            break;

        case(0x6A):
            integer=106;
            break;

        case(0x6B):
            integer=107;
            break;

        case(0x6C):
            integer=108;
            break;

        case(0x6D):
            integer=109;
            break;

        case(0x6E):
            integer=110;
            break;

        case(0x6F):
            integer=111;
            break;


        // 112 - 127


        case(0x70):
            integer=112;
            break;

        case(0x71):
            integer=113;
            break;

        case(0x72):
            integer=114;
            break;

        case(0x73):
            integer=115;
            break;

        case(0x74):
            integer=116;
            break;

        case(0x75):
            integer=117;
            break;

        case(0x76):
            integer=118;
            break;

        case(0x77):
            integer=119;
            break;

        case(0x78):
            integer=120;
            break;

        case(0x79):
            integer=121;
            break;

        case(0x7A):
            integer=122;
            break;

        case(0x7B):
            integer=123;
            break;

        case(0x7C):
            integer=124;
            break;

        case(0x7D):
            integer=125;
            break;

        case(0x7E):
            integer=126;
            break;

        case(0x7F):
            integer=127;
            break;


        // 128 - 143


        case(0x80):
            integer=128;
            break;

        case(0x81):
            integer=129;
            break;

        case(0x82):
            integer=130;
            break;

        case(0x83):
            integer=131;
            break;

        case(0x84):
            integer=132;
            break;

        case(0x85):
            integer=133;
            break;

        case(0x86):
            integer=134;
            break;

        case(0x87):
            integer=135;
            break;

        case(0x88):
            integer=136;
            break;

        case(0x89):
            integer=137;
            break;

        case(0x8A):
            integer=138;
            break;

        case(0x8B):
            integer=139;
            break;

        case(0x8C):
            integer=140;
            break;

        case(0x8D):
            integer=141;
            break;

        case(0x8E):
            integer=142;
            break;

        case(0x8F):
            integer=143;
            break;


        // 144 - 159


        case(0x90):
            integer=144;
            break;

        case(0x91):
            integer=145;
            break;

        case(0x92):
            integer=146;
            break;

        case(0x93):
            integer=147;
            break;

        case(0x94):
            integer=148;
            break;

        case(0x95):
            integer=149;
            break;

        case(0x96):
            integer=150;
            break;

        case(0x97):
            integer=151;
            break;

        case(0x98):
            integer=152;
            break;

        case(0x99):
            integer=153;
            break;

        case(0x9A):
            integer=154;
            break;

        case(0x9B):
            integer=155;
            break;

        case(0x9C):
            integer=156;
            break;

        case(0x9D):
            integer=157;
            break;

        case(0x9E):
            integer=158;
            break;

        case(0x9F):
            integer=159;
            break;


        // 160 - 175


        case(0xA0):
            integer=160;
            break;

        case(0xA1):
            integer=161;
            break;

        case(0xA2):
            integer=162;
            break;

        case(0xA3):
            integer=163;
            break;

        case(0xA4):
            integer=164;
            break;

        case(0xA5):
            integer=165;
            break;

        case(0xA6):
            integer=166;
            break;

        case(0xA7):
            integer=167;
            break;

        case(0xA8):
            integer=168;
            break;

        case(0xA9):
            integer=169;
            break;

        case(0xAA):
            integer=170;
            break;

        case(0xAB):
            integer=171;
            break;

        case(0xAC):
            integer=172;
            break;

        case(0xAD):
            integer=173;
            break;

        case(0xAE):
            integer=174;
            break;

        case(0xAF):
            integer=175;
            break;


        // 176 - 191

        case(0xB0):
            integer=176;
            break;

        case(0xB1):
            integer=177;
            break;

        case(0xB2):
            integer=178;
            break;

        case(0xB3):
            integer=179;
            break;

        case(0xB4):
            integer=180;
            break;

        case(0xB5):
            integer=181;
            break;

        case(0xB6):
            integer=182;
            break;

        case(0xB7):
            integer=183;
            break;

        case(0xB8):
            integer=184;
            break;

        case(0xB9):
            integer=185;
            break;

        case(0xBA):
            integer=186;
            break;

        case(0xBB):
            integer=187;
            break;

        case(0xBC):
            integer=188;
            break;

        case(0xBD):
            integer=189;
            break;

        case(0xBE):
            integer=190;
            break;

        case(0xBF):
            integer=191;
            break;


        // 192 - 107


        case(0xC0):
            integer=192;
            break;

        case(0xC1):
            integer=193;
            break;

        case(0xC2):
            integer=194;
            break;

        case(0xC3):
            integer=195;
            break;

        case(0xC4):
            integer=196;
            break;

        case(0xC5):
            integer=197;
            break;

        case(0xC6):
            integer=198;
            break;

        case(0xC7):
            integer=199;
            break;

        case(0xC8):
            integer=200;
            break;

        case(0xC9):
            integer=201;
            break;

        case(0xCA):
            integer=202;
            break;

        case(0xCB):
            integer=203;
            break;

        case(0xCC):
            integer=204;
            break;

        case(0xCD):
            integer=205;
            break;

        case(0xCE):
            integer=206;
            break;

        case(0xCF):
            integer=207;
            break;


        // 208 - 223


        case(0xD0):
            integer=208;
            break;

        case(0xD1):
            integer=209;
            break;

        case(0xD2):
            integer=210;
            break;

        case(0xD3):
            integer=211;
            break;

        case(0xD4):
            integer=212;
            break;

        case(0xD5):
            integer=213;
            break;

        case(0xD6):
            integer=214;
            break;

        case(0xD7):
            integer=215;
            break;

        case(0xD8):
            integer=216;
            break;

        case(0xD9):
            integer=217;
            break;

        case(0xDA):
            integer=218;
            break;

        case(0xDB):
            integer=219;
            break;

        case(0xDC):
            integer=220;
            break;

        case(0xDD):
            integer=221;
            break;

        case(0xDE):
            integer=222;
            break;

        case(0xDF):
            integer=223;
            break;


        // 224 - 139


        case(0xE0):
            integer=224;
            break;

        case(0xE1):
            integer=225;
            break;

        case(0xE2):
            integer=226;
            break;

        case(0xE3):
            integer=227;
            break;

        case(0xE4):
            integer=228;
            break;

        case(0xE5):
            integer=229;
            break;

        case(0xE6):
            integer=230;
            break;

        case(0xE7):
            integer=231;
            break;

        case(0xE8):
            integer=232;
            break;

        case(0xE9):
            integer=233;
            break;

        case(0xEA):
            integer=234;
            break;

        case(0xEB):
            integer=235;
            break;

        case(0xEC):
            integer=236;
            break;

        case(0xED):
            integer=237;
            break;

        case(0xEE):
            integer=238;
            break;

        case(0xEF):
            integer=239;
            break;


        // 240 - 255


        case(0xF0):
            integer=240;
            break;

        case(0xF1):
            integer=241;
            break;

        case(0xF2):
            integer=242;
            break;

        case(0xF3):
            integer=243;
            break;

        case(0xF4):
            integer=244;
            break;

        case(0xF5):
            integer=245;
            break;

        case(0xF6):
            integer=246;
            break;

        case(0xF7):
            integer=247;
            break;

        case(0xF8):
            integer=248;
            break;

        case(0xF9):
            integer=249;
            break;

        case(0xFA):
            integer=250;
            break;

        case(0xFB):
            integer=251;
            break;

        case(0xFC):
            integer=252;
            break;

        case(0xFD):
            integer=253;
            break;

        case(0xFE):
            integer=254;
            break;

        case(0xFF):
            integer=255;
            break;
    }
}

}//end namespace caching