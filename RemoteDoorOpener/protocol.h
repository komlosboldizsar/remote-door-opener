#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

typedef struct _RemoteProcotol {
	unsigned int baseTime;
	unsigned char isInverted;
	unsigned char syncLength[2];
	unsigned char bitLengths[2][2];
} RemoteProtocol;

const extern RemoteProtocol MY_PROTOCOL;

#endif // PROTOCOL_H_INCLUDED