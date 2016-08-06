char display_decode(char seg) {

	char rc;
	switch(seg)
	{
        case 0b00000101:
		rc=1;
		break;
        case 0b01011011:
		rc=2;
		break;
        case 0b00011111:
		rc=3;
		break;
        case 0b00100111:
		rc=4;
		break;
        case 0b00111110:
		rc=5;
		break;
        case 0b01111110:
		rc=6;
		break;
        case 0b00010101:
		rc=7;
		break;
        case 0b01111111:
		rc=8;
		break;
        case 0b00111111:
		rc=9;
		break;
        case 0b01111101:
		rc=0;
		break;
        case 0b01101000: 
		rc='L';
		break;
	default:
		break;
	}
	return rc;
};
