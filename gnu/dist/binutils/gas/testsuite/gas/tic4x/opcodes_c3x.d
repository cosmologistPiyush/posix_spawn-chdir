#as: -m30 --defsym TEST_C3X=1
#objdump: -d -z
#name: c3x opcodes
#source: opcodes.s

.*: +file format .*c4x.*

Disassembly of section .text:

00000000 <absf_B>:
   0:	00000001.*
   1:	00000000.*
   2:	00200000.*
   3:	00400005.*
   4:	00601600.*

00000005 <absf_stf_P>:
   5:	c8010100.*
   6:	c8010100.*

00000007 <absi_A>:
   7:	00880009.*
   8:	00880008.*
   9:	00a80000.*
   a:	00c80005.*
   b:	00e8fffb.*

0000000c <absi_sti_P>:
   c:	ca010100.*
   d:	ca010100.*

0000000e <addc_A>:
   e:	01080009.*
   f:	01080008.*
  10:	01280000.*
  11:	01480005.*
  12:	0168fffb.*

00000013 <addc_TC>:
  13:	2008090a.*
  14:	01080009.*
  15:	20280009.*
  16:	20480900.*
  17:	01480001.*
  18:	20680001.*

00000019 <addc3_TC>:
  19:	2008090a.*
  1a:	20080809.*
  1b:	20280009.*
  1c:	20480900.*
  1d:	20480800.*
  1e:	20680001.*

0000001f <addf_B>:
  1f:	01800001.*
  20:	01800000.*
  21:	01a00000.*
  22:	01c00005.*
  23:	01e01600.*

00000024 <addf_SC>:
  24:	20800102.*
  25:	01800001.*
  26:	20a00001.*
  27:	20c00100.*
  28:	01c00001.*
  29:	20e00100.*

0000002a <addf3_SC>:
  2a:	20800102.*
  2b:	20800001.*
  2c:	20a00001.*
  2d:	20c00100.*
  2e:	20c00000.*
  2f:	20e00100.*

00000030 <addf_stf_QC>:
  30:	cc090100.*
  31:	cc010100.*
  32:	cc010100.*
  33:	cc090100.*
  34:	cc010100.*
  35:	cc010100.*

00000036 <addf3_stf_QC>:
  36:	cc090100.*
  37:	cc010100.*
  38:	cc010100.*
  39:	cc090100.*
  3a:	cc010100.*
  3b:	cc010100.*

0000003c <addi_A>:
  3c:	02080009.*
  3d:	02080008.*
  3e:	02280000.*
  3f:	02480005.*
  40:	0268fffb.*

00000041 <addi_TC>:
  41:	2108090a.*
  42:	02080009.*
  43:	21280009.*
  44:	21480900.*
  45:	02480001.*
  46:	21680001.*

00000047 <addi3_TC>:
  47:	2108090a.*
  48:	21080809.*
  49:	21280009.*
  4a:	21480900.*
  4b:	21480800.*
  4c:	21680001.*

0000004d <addi_sti_QC>:
  4d:	ce090100.*
  4e:	ce010100.*
  4f:	ce010100.*
  50:	ce090100.*
  51:	ce010100.*
  52:	ce010100.*

00000053 <addi3_sti_QC>:
  53:	ce090100.*
  54:	ce010100.*
  55:	ce010100.*
  56:	ce090100.*
  57:	ce010100.*
  58:	ce010100.*

00000059 <and_AU>:
  59:	02880009.*
  5a:	02880008.*
  5b:	02a80000.*
  5c:	02c80005.*
  5d:	02e80005.*

0000005e <and_TC>:
  5e:	2188090a.*
  5f:	02880009.*
  60:	21a80009.*
  61:	21c80900.*
  62:	02c80001.*
  63:	21e80001.*

00000064 <and3_TC>:
  64:	2188090a.*
  65:	21880809.*
  66:	21a80009.*
  67:	21c80900.*
  68:	21c80800.*
  69:	21e80001.*

0000006a <and_sti_QC>:
  6a:	d0090100.*
  6b:	d0010100.*
  6c:	d0010100.*
  6d:	d0090100.*
  6e:	d0010100.*
  6f:	d0010100.*

00000070 <and3_sti_QC>:
  70:	d0090100.*
  71:	d0010100.*
  72:	d0010100.*
  73:	d0090100.*
  74:	d0010100.*
  75:	d0010100.*

00000076 <andn_AU>:
  76:	03080009.*
  77:	03080008.*
  78:	03280000.*
  79:	03480005.*
  7a:	03680005.*

0000007b <andn_T>:
  7b:	2208090a.*
  7c:	03080009.*
  7d:	22280009.*
  7e:	22480900.*
  7f:	03480001.*
  80:	22680001.*

00000081 <andn3_T>:
  81:	2208090a.*
  82:	22080809.*
  83:	22280009.*
  84:	22480900.*
  85:	22480800.*
  86:	22680001.*

00000087 <ash_A>:
  87:	03880009.*
  88:	03880008.*
  89:	03a80000.*
  8a:	03c80005.*
  8b:	03e8fffb.*

0000008c <ash_T>:
  8c:	2288090a.*
  8d:	03880009.*
  8e:	22a80009.*
  8f:	22c80900.*
  90:	03c80001.*
  91:	22e80001.*

00000092 <ash3_T>:
  92:	2288090a.*
  93:	22880809.*
  94:	22a80009.*
  95:	22c80900.*
  96:	22c80800.*
  97:	22e80001.*

00000098 <ash_sti_Q>:
  98:	d2010100.*
  99:	d2010100.*

0000009a <ash3_sti_Q>:
  9a:	d2010100.*
  9b:	d2010100.*

0000009c <bC_J>:
  9c:	68010000.*
  9d:	6a01ff62.*

0000009e <b_J>:
  9e:	68000000.*
  9f:	6a00ff60.*

000000a0 <bCd_J>:
  a0:	68210000.*
  a1:	6a21ff5c.*

000000a2 <bd_J>:
  a2:	68200000.*
  a3:	6a20ff5a.*

000000a4 <br_I>:
  a4:	60000000.*

000000a5 <brd_I>:
  a5:	61000000.*

000000a6 <call_I>:
  a6:	62000000.*

000000a7 <call_JS>:
  a7:	70010000.*
  a8:	7201ff57.*

000000a9 <cmpf_B>:
  a9:	04000001.*
  aa:	04000000.*
  ab:	04200000.*
  ac:	04400005.*
  ad:	04601600.*

000000ae <cmpf_S2>:
  ae:	04010002.*
  af:	23200001.*
  b0:	04410001.*
  b1:	23600100.*

000000b2 <cmpf3_S2>:
  b2:	23000102.*
  b3:	23200001.*
  b4:	23400100.*
  b5:	23600100.*

000000b6 <cmpi_A>:
  b6:	04880009.*
  b7:	04880008.*
  b8:	04a80000.*
  b9:	04c80005.*
  ba:	04e8fffb.*

000000bb <cmpi_T2>:
  bb:	0489000a.*
  bc:	23a00009.*
  bd:	04c90001.*
  be:	23e00001.*

000000bf <cmpi3_T2>:
  bf:	2380090a.*
  c0:	23a00009.*
  c1:	23c00900.*
  c2:	23e00001.*

000000c3 <dbC_D>:
  c3:	6c010000.*
  c4:	6e01ff3b.*

000000c5 <db_D>:
  c5:	6c000000.*
  c6:	6e00ff39.*

000000c7 <dbCd_D>:
  c7:	6c210000.*
  c8:	6e21ff35.*

000000c9 <dbd_D>:
  c9:	6c200000.*
  ca:	6e20ff33.*

000000cb <fix_AF>:
  cb:	05000001.*
  cc:	05000000.*
  cd:	05280000.*
  ce:	05480005.*
  cf:	05681600.*

000000d0 <fix_sti_P>:
  d0:	d4010100.*
  d1:	d4010100.*

000000d2 <float_BI>:
  d2:	05800009.*
  d3:	05800000.*
  d4:	05a00000.*
  d5:	05c00005.*
  d6:	05e0fffb.*

000000d7 <float_stf_P>:
  d7:	d6010100.*
  d8:	d6010100.*

000000d9 <iack_Z>:
  d9:	1b200000.*
  da:	1b400001.*

000000db <idle_Z>:
  db:	06000000.*

000000dc <lde_B>:
  dc:	06800001.*
  dd:	06800000.*
  de:	06a00000.*
  df:	06c00005.*
  e0:	06e01600.*

000000e1 <ldf_B>:
  e1:	07000001.*
  e2:	07000000.*
  e3:	07200000.*
  e4:	07400005.*
  e5:	07601600.*

000000e6 <ldf_LL>:
  e6:	c4080100.*
  e7:	c4080100.*
  e8:	c4080100.*

000000e9 <ldf_stf_P>:
  e9:	d8010100.*
  ea:	d8010100.*

000000eb <ldfC_BB>:
  eb:	40800001.*
  ec:	40800000.*
  ed:	40a00000.*
  ee:	40c00005.*
  ef:	40e01600.*

000000f0 <ldfi_B6>:
  f0:	07a00000.*
  f1:	07c00005.*

000000f2 <ldi_A>:
  f2:	08080009.*
  f3:	08080008.*
  f4:	08280000.*
  f5:	08480005.*
  f6:	0868fffb.*

000000f7 <ldi_LL>:
  f7:	c6080100.*
  f8:	c6080100.*
  f9:	c6080100.*

000000fa <ldi_sti_P>:
  fa:	da010100.*
  fb:	da010100.*

000000fc <ldiC_AB>:
  fc:	50880009.*
  fd:	50880008.*
  fe:	50a80000.*
  ff:	50c80005.*
 100:	50e8fffb.*

00000101 <ldii_A6>:
 101:	08a80000.*
 102:	08c80005.*

00000103 <ldp_Z>:
 103:	50700000.*

00000104 <ldm_B>:
 104:	09000001.*
 105:	09000000.*
 106:	09200000.*
 107:	09400005.*
 108:	09601600.*

00000109 <lsh_A>:
 109:	09880009.*
 10a:	09880008.*
 10b:	09a80000.*
 10c:	09c80005.*
 10d:	09e8fffb.*

0000010e <lsh_T>:
 10e:	2408090a.*
 10f:	09880009.*
 110:	24280009.*
 111:	24480900.*
 112:	09c80001.*
 113:	24680001.*

00000114 <lsh3_T>:
 114:	2408090a.*
 115:	24080809.*
 116:	24280009.*
 117:	24480900.*
 118:	24480800.*
 119:	24680001.*

0000011a <lsh_sti_Q>:
 11a:	dc010100.*
 11b:	dc010100.*

0000011c <lsh3_sti_Q>:
 11c:	dc010100.*
 11d:	dc010100.*

0000011e <mpyf_B>:
 11e:	0a000001.*
 11f:	0a000000.*
 120:	0a200000.*
 121:	0a400005.*
 122:	0a601600.*

00000123 <mpyf_SC>:
 123:	24800102.*
 124:	0a000001.*
 125:	24a00001.*
 126:	24c00100.*
 127:	0a400001.*
 128:	24e00100.*

00000129 <mpyf3_SC>:
 129:	24800102.*
 12a:	24800001.*
 12b:	24a00001.*
 12c:	24c00100.*
 12d:	24c00000.*
 12e:	24e00100.*

0000012f <mpyf_addf_M>:
 12f:	80080100.*
 130:	80100100.*
 131:	81000001.*
 132:	81000001.*
 133:	81000001.*
 134:	820a0100.*
 135:	82020100.*
 136:	830b0001.*
 137:	83030001.*
 138:	830a0001.*
 139:	83020001.*
 13a:	83000001.*
 13b:	83020001.*

0000013c <mpyf3_addf_M>:
 13c:	80080100.*
 13d:	80100100.*
 13e:	81000001.*
 13f:	81000001.*
 140:	81000001.*
 141:	820a0100.*
 142:	82020100.*
 143:	830b0001.*
 144:	83030001.*
 145:	830a0001.*
 146:	83020001.*
 147:	83000001.*
 148:	83020001.*

00000149 <mpyf_addf3_M>:
 149:	80080100.*
 14a:	80100100.*
 14b:	81000001.*
 14c:	81000001.*
 14d:	81000001.*
 14e:	820a0100.*
 14f:	82020100.*
 150:	830b0001.*
 151:	83030001.*
 152:	830a0001.*
 153:	83020001.*
 154:	83000001.*
 155:	83020001.*

00000156 <mpyf3_addf3_M>:
 156:	80080100.*
 157:	80100100.*
 158:	81000001.*
 159:	81000001.*
 15a:	81000001.*
 15b:	820a0100.*
 15c:	82020100.*
 15d:	830b0001.*
 15e:	83030001.*
 15f:	830a0001.*
 160:	83020001.*
 161:	83000001.*
 162:	83020001.*

00000163 <addf_mpyf_M>:
 163:	80080100.*
 164:	80100100.*
 165:	81000001.*
 166:	81000001.*
 167:	81000001.*
 168:	820a0100.*
 169:	82020100.*
 16a:	830b0001.*
 16b:	83030001.*
 16c:	830a0001.*
 16d:	83020001.*
 16e:	83000001.*
 16f:	83020001.*

00000170 <addf3_mpyf_M>:
 170:	80080100.*
 171:	80100100.*
 172:	81000001.*
 173:	81000001.*
 174:	81000001.*
 175:	820a0100.*
 176:	82020100.*
 177:	830b0001.*
 178:	83030001.*
 179:	830a0001.*
 17a:	83020001.*
 17b:	83000001.*
 17c:	83020001.*

0000017d <addf_mpyf3_M>:
 17d:	80080100.*
 17e:	80100100.*
 17f:	81000001.*
 180:	81000001.*
 181:	81000001.*
 182:	820a0100.*
 183:	82020100.*
 184:	830b0001.*
 185:	83030001.*
 186:	830a0001.*
 187:	83020001.*
 188:	83000001.*
 189:	83020001.*

0000018a <addf3_mpyf3_M>:
 18a:	80080100.*
 18b:	80100100.*
 18c:	81000001.*
 18d:	81000001.*
 18e:	81000001.*
 18f:	820a0100.*
 190:	82020100.*
 191:	830b0001.*
 192:	83030001.*
 193:	830a0001.*
 194:	83020001.*
 195:	83000001.*
 196:	83020001.*

00000197 <mpyf_stf_QC>:
 197:	de090100.*
 198:	de010100.*
 199:	de010100.*
 19a:	de090100.*
 19b:	de010100.*
 19c:	de010100.*

0000019d <mpyf3_stf_QC>:
 19d:	de090100.*
 19e:	de010100.*
 19f:	de010100.*
 1a0:	de090100.*
 1a1:	de010100.*
 1a2:	de010100.*

000001a3 <mpyf_subf_M>:
 1a3:	84080100.*
 1a4:	84100100.*
 1a5:	85000001.*
 1a6:	85000001.*
 1a7:	85000001.*
 1a8:	860a0100.*
 1a9:	86020100.*
 1aa:	870b0001.*
 1ab:	87030001.*
 1ac:	870a0001.*
 1ad:	87020001.*
 1ae:	87000001.*
 1af:	87020001.*

000001b0 <mpyf3_subf_M>:
 1b0:	84080100.*
 1b1:	84100100.*
 1b2:	85000001.*
 1b3:	85000001.*
 1b4:	85000001.*
 1b5:	860a0100.*
 1b6:	86020100.*
 1b7:	870b0001.*
 1b8:	87030001.*
 1b9:	870a0001.*
 1ba:	87020001.*
 1bb:	87000001.*
 1bc:	87020001.*

000001bd <mpyf_subf3_M>:
 1bd:	84080100.*
 1be:	84100100.*
 1bf:	85000001.*
 1c0:	85000001.*
 1c1:	85000001.*
 1c2:	860a0100.*
 1c3:	86020100.*
 1c4:	870b0001.*
 1c5:	87030001.*
 1c6:	870a0001.*
 1c7:	87020001.*
 1c8:	87000001.*
 1c9:	87020001.*

000001ca <mpyf3_subf3_M>:
 1ca:	84080100.*
 1cb:	84100100.*
 1cc:	85000001.*
 1cd:	85000001.*
 1ce:	85000001.*
 1cf:	860a0100.*
 1d0:	86020100.*
 1d1:	870b0001.*
 1d2:	87030001.*
 1d3:	870a0001.*
 1d4:	87020001.*
 1d5:	87000001.*
 1d6:	87020001.*

000001d7 <subf_mpyf_M>:
 1d7:	84080100.*
 1d8:	84100100.*
 1d9:	85000001.*
 1da:	85000001.*
 1db:	85000001.*
 1dc:	860a0100.*
 1dd:	86020100.*
 1de:	870b0001.*
 1df:	87030001.*
 1e0:	870a0001.*
 1e1:	87020001.*
 1e2:	87000001.*
 1e3:	87020001.*

000001e4 <subf3_mpyf_M>:
 1e4:	84080100.*
 1e5:	84100100.*
 1e6:	85000001.*
 1e7:	85000001.*
 1e8:	85000001.*
 1e9:	860a0100.*
 1ea:	86020100.*
 1eb:	870b0001.*
 1ec:	87030001.*
 1ed:	870a0001.*
 1ee:	87020001.*
 1ef:	87000001.*
 1f0:	87020001.*

000001f1 <subf_mpyf3_M>:
 1f1:	84080100.*
 1f2:	84100100.*
 1f3:	85000001.*
 1f4:	85000001.*
 1f5:	85000001.*
 1f6:	860a0100.*
 1f7:	86020100.*
 1f8:	870b0001.*
 1f9:	87030001.*
 1fa:	870a0001.*
 1fb:	87020001.*
 1fc:	87000001.*
 1fd:	87020001.*

000001fe <subf3_mpyf3_M>:
 1fe:	84080100.*
 1ff:	84100100.*
 200:	85000001.*
 201:	85000001.*
 202:	85000001.*
 203:	860a0100.*
 204:	86020100.*
 205:	870b0001.*
 206:	87030001.*
 207:	870a0001.*
 208:	87020001.*
 209:	87000001.*
 20a:	87020001.*

0000020b <mpyi_A>:
 20b:	0a880009.*
 20c:	0a880008.*
 20d:	0aa80000.*
 20e:	0ac80005.*
 20f:	0ae8fffb.*

00000210 <mpyi_TC>:
 210:	2508090a.*
 211:	0a880009.*
 212:	25280009.*
 213:	25480900.*
 214:	0ac80001.*
 215:	25680001.*

00000216 <mpyi3_TC>:
 216:	2508090a.*
 217:	25080809.*
 218:	25280009.*
 219:	25480900.*
 21a:	25480800.*
 21b:	25680001.*

0000021c <mpyi_addi_M>:
 21c:	88080100.*
 21d:	88100100.*
 21e:	89000001.*
 21f:	89000001.*
 220:	89000001.*
 221:	8a0a0100.*
 222:	8a020100.*
 223:	8b0b0001.*
 224:	8b030001.*
 225:	8b0a0001.*
 226:	8b020001.*
 227:	8b000001.*
 228:	8b020001.*

00000229 <mpyi3_addi_M>:
 229:	88080100.*
 22a:	88100100.*
 22b:	89000001.*
 22c:	89000001.*
 22d:	89000001.*
 22e:	8a0a0100.*
 22f:	8a020100.*
 230:	8b0b0001.*
 231:	8b030001.*
 232:	8b0a0001.*
 233:	8b020001.*
 234:	8b000001.*
 235:	8b020001.*

00000236 <mpyi_addi3_M>:
 236:	88080100.*
 237:	88100100.*
 238:	89000001.*
 239:	89000001.*
 23a:	89000001.*
 23b:	8a0a0100.*
 23c:	8a020100.*
 23d:	8b0b0001.*
 23e:	8b030001.*
 23f:	8b0a0001.*
 240:	8b020001.*
 241:	8b000001.*
 242:	8b020001.*

00000243 <mpyi3_addi3_M>:
 243:	88080100.*
 244:	88100100.*
 245:	89000001.*
 246:	89000001.*
 247:	89000001.*
 248:	8a0a0100.*
 249:	8a020100.*
 24a:	8b0b0001.*
 24b:	8b030001.*
 24c:	8b0a0001.*
 24d:	8b020001.*
 24e:	8b000001.*
 24f:	8b020001.*

00000250 <addi_mpyi_M>:
 250:	88080100.*
 251:	88100100.*
 252:	89000001.*
 253:	89000001.*
 254:	89000001.*
 255:	8a0a0100.*
 256:	8a020100.*
 257:	8b0b0001.*
 258:	8b030001.*
 259:	8b0a0001.*
 25a:	8b020001.*
 25b:	8b000001.*
 25c:	8b020001.*

0000025d <addi3_mpyi_M>:
 25d:	88080100.*
 25e:	88100100.*
 25f:	89000001.*
 260:	89000001.*
 261:	89000001.*
 262:	8a0a0100.*
 263:	8a020100.*
 264:	8b0b0001.*
 265:	8b030001.*
 266:	8b0a0001.*
 267:	8b020001.*
 268:	8b000001.*
 269:	8b020001.*

0000026a <addi_mpyi3_M>:
 26a:	88080100.*
 26b:	88100100.*
 26c:	89000001.*
 26d:	89000001.*
 26e:	89000001.*
 26f:	8a0a0100.*
 270:	8a020100.*
 271:	8b0b0001.*
 272:	8b030001.*
 273:	8b0a0001.*
 274:	8b020001.*
 275:	8b000001.*
 276:	8b020001.*

00000277 <addi3_mpyi3_M>:
 277:	88080100.*
 278:	88100100.*
 279:	89000001.*
 27a:	89000001.*
 27b:	89000001.*
 27c:	8a0a0100.*
 27d:	8a020100.*
 27e:	8b0b0001.*
 27f:	8b030001.*
 280:	8b0a0001.*
 281:	8b020001.*
 282:	8b000001.*
 283:	8b020001.*

00000284 <mpyi_sti_QC>:
 284:	e0090100.*
 285:	e0010100.*
 286:	e0010100.*
 287:	e0090100.*
 288:	e0010100.*
 289:	e0010100.*

0000028a <mpyi3_sti_QC>:
 28a:	e0090100.*
 28b:	e0010100.*
 28c:	e0010100.*
 28d:	e0090100.*
 28e:	e0010100.*
 28f:	e0010100.*

00000290 <mpyi_subi_M>:
 290:	8c080100.*
 291:	8c100100.*
 292:	8d000001.*
 293:	8d000001.*
 294:	8d000001.*
 295:	8e0a0100.*
 296:	8e020100.*
 297:	8f0b0001.*
 298:	8f030001.*
 299:	8f0a0001.*
 29a:	8f020001.*
 29b:	8f000001.*
 29c:	8f020001.*

0000029d <mpyi3_subi_M>:
 29d:	8c080100.*
 29e:	8c100100.*
 29f:	8d000001.*
 2a0:	8d000001.*
 2a1:	8d000001.*
 2a2:	8e0a0100.*
 2a3:	8e020100.*
 2a4:	8f0b0001.*
 2a5:	8f030001.*
 2a6:	8f0a0001.*
 2a7:	8f020001.*
 2a8:	8f000001.*
 2a9:	8f020001.*

000002aa <mpyi_subi3_M>:
 2aa:	8c080100.*
 2ab:	8c100100.*
 2ac:	8d000001.*
 2ad:	8d000001.*
 2ae:	8d000001.*
 2af:	8e0a0100.*
 2b0:	8e020100.*
 2b1:	8f0b0001.*
 2b2:	8f030001.*
 2b3:	8f0a0001.*
 2b4:	8f020001.*
 2b5:	8f000001.*
 2b6:	8f020001.*

000002b7 <mpyi3_subi3_M>:
 2b7:	8c080100.*
 2b8:	8c100100.*
 2b9:	8d000001.*
 2ba:	8d000001.*
 2bb:	8d000001.*
 2bc:	8e0a0100.*
 2bd:	8e020100.*
 2be:	8f0b0001.*
 2bf:	8f030001.*
 2c0:	8f0a0001.*
 2c1:	8f020001.*
 2c2:	8f000001.*
 2c3:	8f020001.*

000002c4 <subi_mpyi_M>:
 2c4:	8c080100.*
 2c5:	8c100100.*
 2c6:	8d000001.*
 2c7:	8d000001.*
 2c8:	8d000001.*
 2c9:	8e0a0100.*
 2ca:	8e020100.*
 2cb:	8f0b0001.*
 2cc:	8f030001.*
 2cd:	8f0a0001.*
 2ce:	8f020001.*
 2cf:	8f000001.*
 2d0:	8f020001.*

000002d1 <subi3_mpyi_M>:
 2d1:	8c080100.*
 2d2:	8c100100.*
 2d3:	8d000001.*
 2d4:	8d000001.*
 2d5:	8d000001.*
 2d6:	8e0a0100.*
 2d7:	8e020100.*
 2d8:	8f0b0001.*
 2d9:	8f030001.*
 2da:	8f0a0001.*
 2db:	8f020001.*
 2dc:	8f000001.*
 2dd:	8f020001.*

000002de <subi_mpyi3_M>:
 2de:	8c080100.*
 2df:	8c100100.*
 2e0:	8d000001.*
 2e1:	8d000001.*
 2e2:	8d000001.*
 2e3:	8e0a0100.*
 2e4:	8e020100.*
 2e5:	8f0b0001.*
 2e6:	8f030001.*
 2e7:	8f0a0001.*
 2e8:	8f020001.*
 2e9:	8f000001.*
 2ea:	8f020001.*

000002eb <subi3_mpyi3_M>:
 2eb:	8c080100.*
 2ec:	8c100100.*
 2ed:	8d000001.*
 2ee:	8d000001.*
 2ef:	8d000001.*
 2f0:	8e0a0100.*
 2f1:	8e020100.*
 2f2:	8f0b0001.*
 2f3:	8f030001.*
 2f4:	8f0a0001.*
 2f5:	8f020001.*
 2f6:	8f000001.*
 2f7:	8f020001.*

000002f8 <negb_A>:
 2f8:	0b080009.*
 2f9:	0b080008.*
 2fa:	0b280000.*
 2fb:	0b480005.*
 2fc:	0b68fffb.*

000002fd <negf_B>:
 2fd:	0b800001.*
 2fe:	0b800000.*
 2ff:	0ba00000.*
 300:	0bc00005.*
 301:	0be01600.*

00000302 <negf_stf_P>:
 302:	e2010100.*
 303:	e2010100.*

00000304 <negi_A>:
 304:	0c080009.*
 305:	0c080008.*
 306:	0c280000.*
 307:	0c480005.*
 308:	0c68fffb.*

00000309 <negi_sti_P>:
 309:	e4010100.*
 30a:	e4010100.*

0000030b <nop_A2>:
 30b:	0c800008.*
 30c:	0cc00005.*
 30d:	0c800000.*

0000030e <norm_B>:
 30e:	0d000001.*
 30f:	0d000000.*
 310:	0d200000.*
 311:	0d400005.*
 312:	0d601600.*

00000313 <not_AU>:
 313:	0d880009.*
 314:	0d880008.*
 315:	0da80000.*
 316:	0dc80005.*
 317:	0de80005.*

00000318 <not_sti_P>:
 318:	e6010100.*
 319:	e6010100.*

0000031a <or_AU>:
 31a:	10080009.*
 31b:	10080008.*
 31c:	10280000.*
 31d:	10480005.*
 31e:	10680005.*

0000031f <or_TC>:
 31f:	2588090a.*
 320:	10080009.*
 321:	25a80009.*
 322:	25c80900.*
 323:	10480001.*
 324:	25e80001.*

00000325 <or3_TC>:
 325:	2588090a.*
 326:	25880809.*
 327:	25a80009.*
 328:	25c80900.*
 329:	25c80800.*
 32a:	25e80001.*

0000032b <or_sti_QC>:
 32b:	e8090100.*
 32c:	e8010100.*
 32d:	e8010100.*
 32e:	e8090100.*
 32f:	e8010100.*
 330:	e8010100.*

00000331 <or3_sti_QC>:
 331:	e8090100.*
 332:	e8010100.*
 333:	e8010100.*
 334:	e8090100.*
 335:	e8010100.*
 336:	e8010100.*

00000337 <pop_R>:
 337:	0e280000.*

00000338 <popf_RF>:
 338:	0ea00000.*

00000339 <push_R>:
 339:	0f280000.*

0000033a <pushf_RF>:
 33a:	0fa00000.*

0000033b <reti_Z>:
 33b:	78010000.*
 33c:	78000000.*

0000033d <rets_Z>:
 33d:	78810000.*
 33e:	78800000.*

0000033f <rnd_B>:
 33f:	11000001.*
 340:	11000000.*
 341:	11200000.*
 342:	11400005.*
 343:	11601600.*

00000344 <rol_R>:
 344:	11e80001.*

00000345 <rolc_R>:
 345:	12680001.*

00000346 <ror_R>:
 346:	12e8ffff.*

00000347 <rorc_R>:
 347:	1368ffff.*

00000348 <rptb_I2>:
 348:	64000000.*

00000349 <rpts_A3>:
 349:	139b0009.*
 34a:	13bb0000.*
 34b:	13db0005.*
 34c:	13fb0005.*

0000034d <sigi_Z>:
 34d:	16000000.*

0000034e <stf_B7>:
 34e:	14200000.*
 34f:	14400005.*

00000350 <stf_LS>:
 350:	c0010100.*
 351:	c0010100.*
 352:	c0010100.*

00000353 <stfi_B7>:
 353:	14a00000.*
 354:	14c00005.*

00000355 <sti_A7>:
 355:	15280000.*
 356:	15480005.*

00000357 <sti_LS>:
 357:	c2010100.*
 358:	c2010100.*
 359:	c2010100.*

0000035a <stii_A7>:
 35a:	15a80000.*
 35b:	15c80005.*

0000035c <subb_A>:
 35c:	16880009.*
 35d:	16880008.*
 35e:	16a80000.*
 35f:	16c80005.*
 360:	16e8fffb.*

00000361 <subb_T>:
 361:	2608090a.*
 362:	16880009.*
 363:	26280009.*
 364:	26480900.*
 365:	16c80001.*
 366:	26680001.*

00000367 <subb3_T>:
 367:	2608090a.*
 368:	26080809.*
 369:	26280009.*
 36a:	26480900.*
 36b:	26480800.*
 36c:	26680001.*

0000036d <subc_A>:
 36d:	17080009.*
 36e:	17080008.*
 36f:	17280000.*
 370:	17480005.*
 371:	1768fffb.*

00000372 <subf_B>:
 372:	17800001.*
 373:	17800000.*
 374:	17a00000.*
 375:	17c00005.*
 376:	17e01600.*

00000377 <subf_S>:
 377:	26800102.*
 378:	17800001.*
 379:	26a00001.*
 37a:	26c00100.*
 37b:	17c00001.*
 37c:	26e00100.*

0000037d <subf3_S>:
 37d:	26800102.*
 37e:	26800001.*
 37f:	26a00001.*
 380:	26c00100.*
 381:	26c00000.*
 382:	26e00100.*

00000383 <subf_stf_Q>:
 383:	ea010100.*
 384:	ea010100.*

00000385 <subf3_stf_Q>:
 385:	ea010100.*
 386:	ea010100.*

00000387 <subi_A>:
 387:	18080009.*
 388:	18080008.*
 389:	18280000.*
 38a:	18480005.*
 38b:	1868fffb.*

0000038c <subi_T>:
 38c:	2708090a.*
 38d:	18080009.*
 38e:	27280009.*
 38f:	27480900.*
 390:	18480001.*
 391:	27680001.*

00000392 <subi3_T>:
 392:	2708090a.*
 393:	27080809.*
 394:	27280009.*
 395:	27480900.*
 396:	27480800.*
 397:	27680001.*

00000398 <subi_sti_Q>:
 398:	ec010100.*
 399:	ec010100.*

0000039a <subi3_sti_Q>:
 39a:	ec010100.*
 39b:	ec010100.*

0000039c <subrb_A>:
 39c:	18880009.*
 39d:	18880008.*
 39e:	18a80000.*
 39f:	18c80005.*
 3a0:	18e8fffb.*

000003a1 <subrf_B>:
 3a1:	19000001.*
 3a2:	19000000.*
 3a3:	19200000.*
 3a4:	19400005.*
 3a5:	19601600.*

000003a6 <subri_A>:
 3a6:	19880009.*
 3a7:	19880008.*
 3a8:	19a80000.*
 3a9:	19c80005.*
 3aa:	19e8fffb.*

000003ab <swi_Z>:
 3ab:	66000000.*

000003ac <trap_Z>:
 3ac:	7401002a.*
 3ad:	7400002a.*

000003ae <tstb_AU>:
 3ae:	1a080009.*
 3af:	1a080008.*
 3b0:	1a280000.*
 3b1:	1a480005.*
 3b2:	1a680005.*

000003b3 <tstb_T2C>:
 3b3:	1a09000a.*
 3b4:	27a00009.*
 3b5:	1a490001.*
 3b6:	27e00001.*

000003b7 <tstb3_T2C>:
 3b7:	2780090a.*
 3b8:	27a00009.*
 3b9:	27c00900.*
 3ba:	27e00001.*

000003bb <xor_AU>:
 3bb:	1a880009.*
 3bc:	1a880008.*
 3bd:	1aa80000.*
 3be:	1ac80005.*
 3bf:	1ae80005.*

000003c0 <xor_TC>:
 3c0:	2808090a.*
 3c1:	1a880009.*
 3c2:	28280009.*
 3c3:	28480900.*
 3c4:	1ac80001.*
 3c5:	28680001.*

000003c6 <xor3_TC>:
 3c6:	2808090a.*
 3c7:	28080809.*
 3c8:	28280009.*
 3c9:	28480900.*
 3ca:	28480800.*
 3cb:	28680001.*

000003cc <xor_sti_QC>:
 3cc:	ee090100.*
 3cd:	ee010100.*
 3ce:	ee010100.*
 3cf:	ee090100.*
 3d0:	ee010100.*
 3d1:	ee010100.*

000003d2 <xor3_sti_QC>:
 3d2:	ee090100.*
 3d3:	ee010100.*
 3d4:	ee010100.*
 3d5:	ee090100.*
 3d6:	ee010100.*
 3d7:	ee010100.*
