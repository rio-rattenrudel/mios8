// $Id$
const unsigned int lfo_add_table[256] = {
      0, //   0:  0.000 Hz
      1, //   1:  0.015 Hz
      2, //   2:  0.031 Hz
      3, //   3:  0.046 Hz
      4, //   4:  0.061 Hz
      6, //   5:  0.092 Hz
      7, //   6:  0.107 Hz
      8, //   7:  0.122 Hz
     10, //   8:  0.153 Hz
     11, //   9:  0.168 Hz
     12, //  10:  0.183 Hz
     14, //  11:  0.214 Hz
     15, //  12:  0.229 Hz
     17, //  13:  0.259 Hz
     18, //  14:  0.275 Hz
     20, //  15:  0.305 Hz
     21, //  16:  0.320 Hz
     23, //  17:  0.351 Hz
     25, //  18:  0.381 Hz
     26, //  19:  0.397 Hz
     28, //  20:  0.427 Hz
     30, //  21:  0.458 Hz
     31, //  22:  0.473 Hz
     33, //  23:  0.504 Hz
     35, //  24:  0.534 Hz
     37, //  25:  0.565 Hz
     39, //  26:  0.595 Hz
     41, //  27:  0.626 Hz
     42, //  28:  0.641 Hz
     44, //  29:  0.671 Hz
     46, //  30:  0.702 Hz
     48, //  31:  0.732 Hz
     51, //  32:  0.778 Hz
     53, //  33:  0.809 Hz
     55, //  34:  0.839 Hz
     57, //  35:  0.870 Hz
     59, //  36:  0.900 Hz
     62, //  37:  0.946 Hz
     64, //  38:  0.977 Hz
     66, //  39:  1.007 Hz
     69, //  40:  1.053 Hz
     71, //  41:  1.083 Hz
     74, //  42:  1.129 Hz
     76, //  43:  1.160 Hz
     79, //  44:  1.205 Hz
     81, //  45:  1.236 Hz
     84, //  46:  1.282 Hz
     87, //  47:  1.328 Hz
     89, //  48:  1.358 Hz
     92, //  49:  1.404 Hz
     95, //  50:  1.450 Hz
     98, //  51:  1.495 Hz
    101, //  52:  1.541 Hz
    104, //  53:  1.587 Hz
    107, //  54:  1.633 Hz
    110, //  55:  1.678 Hz
    114, //  56:  1.740 Hz
    117, //  57:  1.785 Hz
    120, //  58:  1.831 Hz
    124, //  59:  1.892 Hz
    127, //  60:  1.938 Hz
    130, //  61:  1.984 Hz
    134, //  62:  2.045 Hz
    138, //  63:  2.106 Hz
    141, //  64:  2.151 Hz
    145, //  65:  2.213 Hz
    149, //  66:  2.274 Hz
    153, //  67:  2.335 Hz
    157, //  68:  2.396 Hz
    161, //  69:  2.457 Hz
    165, //  70:  2.518 Hz
    169, //  71:  2.579 Hz
    173, //  72:  2.640 Hz
    178, //  73:  2.716 Hz
    182, //  74:  2.777 Hz
    187, //  75:  2.853 Hz
    191, //  76:  2.914 Hz
    196, //  77:  2.991 Hz
    201, //  78:  3.067 Hz
    206, //  79:  3.143 Hz
    211, //  80:  3.220 Hz
    216, //  81:  3.296 Hz
    221, //  82:  3.372 Hz
    226, //  83:  3.448 Hz
    231, //  84:  3.525 Hz
    237, //  85:  3.616 Hz
    242, //  86:  3.693 Hz
    248, //  87:  3.784 Hz
    253, //  88:  3.860 Hz
    259, //  89:  3.952 Hz
    265, //  90:  4.044 Hz
    271, //  91:  4.135 Hz
    277, //  92:  4.227 Hz
    283, //  93:  4.318 Hz
    290, //  94:  4.425 Hz
    296, //  95:  4.517 Hz
    303, //  96:  4.623 Hz
    310, //  97:  4.730 Hz
    316, //  98:  4.822 Hz
    323, //  99:  4.929 Hz
    330, // 100:  5.035 Hz
    338, // 101:  5.157 Hz
    345, // 102:  5.264 Hz
    352, // 103:  5.371 Hz
    360, // 104:  5.493 Hz
    368, // 105:  5.615 Hz
    376, // 106:  5.737 Hz
    384, // 107:  5.859 Hz
    392, // 108:  5.981 Hz
    400, // 109:  6.104 Hz
    409, // 110:  6.241 Hz
    417, // 111:  6.363 Hz
    426, // 112:  6.500 Hz
    435, // 113:  6.638 Hz
    444, // 114:  6.775 Hz
    453, // 115:  6.912 Hz
    463, // 116:  7.065 Hz
    472, // 117:  7.202 Hz
    482, // 118:  7.355 Hz
    492, // 119:  7.507 Hz
    502, // 120:  7.660 Hz
    513, // 121:  7.828 Hz
    523, // 122:  7.980 Hz
    534, // 123:  8.148 Hz
    545, // 124:  8.316 Hz
    556, // 125:  8.484 Hz
    567, // 126:  8.652 Hz
    579, // 127:  8.835 Hz
    590, // 128:  9.003 Hz
    602, // 129:  9.186 Hz
    614, // 130:  9.369 Hz
    627, // 131:  9.567 Hz
    639, // 132:  9.750 Hz
    652, // 133:  9.949 Hz
    665, // 134: 10.147 Hz
    678, // 135: 10.345 Hz
    692, // 136: 10.559 Hz
    706, // 137: 10.773 Hz
    720, // 138: 10.986 Hz
    734, // 139: 11.200 Hz
    748, // 140: 11.414 Hz
    763, // 141: 11.642 Hz
    778, // 142: 11.871 Hz
    794, // 143: 12.115 Hz
    809, // 144: 12.344 Hz
    825, // 145: 12.589 Hz
    841, // 146: 12.833 Hz
    858, // 147: 13.092 Hz
    875, // 148: 13.351 Hz
    892, // 149: 13.611 Hz
    909, // 150: 13.870 Hz
    927, // 151: 14.145 Hz
    945, // 152: 14.420 Hz
    963, // 153: 14.694 Hz
    982, // 154: 14.984 Hz
   1001, // 155: 15.274 Hz
   1020, // 156: 15.564 Hz
   1040, // 157: 15.869 Hz
   1060, // 158: 16.174 Hz
   1081, // 159: 16.495 Hz
   1101, // 160: 16.800 Hz
   1123, // 161: 17.136 Hz
   1144, // 162: 17.456 Hz
   1166, // 163: 17.792 Hz
   1189, // 164: 18.143 Hz
   1211, // 165: 18.478 Hz
   1235, // 166: 18.845 Hz
   1258, // 167: 19.196 Hz
   1282, // 168: 19.562 Hz
   1307, // 169: 19.943 Hz
   1332, // 170: 20.325 Hz
   1357, // 171: 20.706 Hz
   1383, // 172: 21.103 Hz
   1409, // 173: 21.500 Hz
   1436, // 174: 21.912 Hz
   1463, // 175: 22.324 Hz
   1491, // 176: 22.751 Hz
   1519, // 177: 23.178 Hz
   1548, // 178: 23.621 Hz
   1577, // 179: 24.063 Hz
   1607, // 180: 24.521 Hz
   1638, // 181: 24.994 Hz
   1669, // 182: 25.467 Hz
   1700, // 183: 25.940 Hz
   1732, // 184: 26.428 Hz
   1765, // 185: 26.932 Hz
   1798, // 186: 27.435 Hz
   1832, // 187: 27.954 Hz
   1867, // 188: 28.488 Hz
   1902, // 189: 29.022 Hz
   1937, // 190: 29.556 Hz
   1974, // 191: 30.121 Hz
   2011, // 192: 30.685 Hz
   2049, // 193: 31.265 Hz
   2087, // 194: 31.845 Hz
   2126, // 195: 32.440 Hz
   2166, // 196: 33.051 Hz
   2206, // 197: 33.661 Hz
   2248, // 198: 34.302 Hz
   2290, // 199: 34.943 Hz
   2332, // 200: 35.583 Hz
   2376, // 201: 36.255 Hz
   2420, // 202: 36.926 Hz
   2466, // 203: 37.628 Hz
   2512, // 204: 38.330 Hz
   2558, // 205: 39.032 Hz
   2606, // 206: 39.764 Hz
   2655, // 207: 40.512 Hz
   2704, // 208: 41.260 Hz
   2754, // 209: 42.023 Hz
   2805, // 210: 42.801 Hz
   2858, // 211: 43.610 Hz
   2911, // 212: 44.418 Hz
   2965, // 213: 45.242 Hz
   3020, // 214: 46.082 Hz
   3076, // 215: 46.936 Hz
   3133, // 216: 47.806 Hz
   3191, // 217: 48.691 Hz
   3250, // 218: 49.591 Hz
   3310, // 219: 50.507 Hz
   3372, // 220: 51.453 Hz
   3434, // 221: 52.399 Hz
   3498, // 222: 53.375 Hz
   3563, // 223: 54.367 Hz
   3628, // 224: 55.359 Hz
   3696, // 225: 56.396 Hz
   3764, // 226: 57.434 Hz
   3833, // 227: 58.487 Hz
   3904, // 228: 59.570 Hz
   3976, // 229: 60.669 Hz
   4050, // 230: 61.798 Hz
   4125, // 231: 62.943 Hz
   4201, // 232: 64.102 Hz
   4278, // 233: 65.277 Hz
   4357, // 234: 66.483 Hz
   4437, // 235: 67.703 Hz
   4519, // 236: 68.954 Hz
   4602, // 237: 70.221 Hz
   4687, // 238: 71.518 Hz
   4774, // 239: 72.845 Hz
   4862, // 240: 74.188 Hz
   4951, // 241: 75.546 Hz
   5042, // 242: 76.935 Hz
   5135, // 243: 78.354 Hz
   5229, // 244: 79.788 Hz
   5326, // 245: 81.268 Hz
   5424, // 246: 82.764 Hz
   5523, // 247: 84.274 Hz
   5625, // 248: 85.831 Hz
   5728, // 249: 87.402 Hz
   5833, // 250: 89.005 Hz
   5940, // 251: 90.637 Hz
   6050, // 252: 92.316 Hz
   6161, // 253: 94.009 Hz
   6274, // 254: 95.734 Hz
   6389, // 255: 97.488 Hz
};
