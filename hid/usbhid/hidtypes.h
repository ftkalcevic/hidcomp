#ifndef _HIDTYPES_H_
#define _HIDTYPES_H_

#pragma pack(1)

struct HID_DESCRIPTOR
{
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdHID;
    unsigned char bCountryCode;
    unsigned char bNumDescriptors;
    struct
    {
        unsigned char bDescriptorType;
        unsigned short wDescriptorLength;
    } Descriptor[1];
};

#pragma pack()


#endif
