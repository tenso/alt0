#ifndef ALT0CMD_H
#define ALT0CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#define ALT0_NUM_GEN 3
#define ALT0_NUM_LFO 6 //2*NUM_GEN

#define CMD_BYTES 6

    //alt0 always accepts and respons with 6 bytes, 
    //responses are CMD_RET_...
    //one command 6 bytes:
    // cmdTypeId | groupTypeId | param_byte(CmdSysId...) | id_byte | data_high | data_low
    //example: set hz of osc 2 to 110: cmdMake(CMD_SET, CMD_OSC, CMD_OSC_HZ, 2, 110);


    //first byte
    enum CmdTypeId {CMD_SET=0, CMD_GET, CMD_RET_GET, CMD_RET_CMD_FAILED, CMD_RET_CMD_SET_FAILED,
                    CMD_RET_CMD_GET_FAILED, CMD_RET_CMD_OK, CMD_RET_INFO, CMD_RET_BOOT_OK};
    typedef enum CmdTypeId CmdTypeId;


    //second byte
    enum CmdGroupId {CMD_SYS=0, CMD_OSC, CMD_ENV, CMD_LP, CMD_DFB, CMD_LFO, CMD_ANALOG, CMD_HW_INPUT, CMD_GROUP_SIZE};
    typedef enum CmdGroupId CmdGroupId;


    //third byte
    enum CmdSysId {CMD_SYS_LS=0, CMD_SYS_INFO};

    enum CmdOscId {CMD_OSC_HZ=0, CMD_OSC_ON, CMD_OSC_AMP, CMD_OSC_WAVE, CMD_OSC_WS, CMD_OSC_PHASE, CMD_OSC_GLIDE};

    enum CmdEnvId {CMD_ENV_ON=0, CMD_ENV_PASS, CMD_ENV_A, CMD_ENV_D, CMD_ENV_S, CMD_ENV_R};

    enum CmdLpId {CMD_LP_CUT=0, CMD_LP_RES, CMD_LP_ENV_ON, CMD_LP_ENV_PASS, CMD_LP_ENV_A, CMD_LP_ENV_D, 
                  CMD_LP_ENV_S, CMD_LP_ENV_R};

    enum CmdDfbId {CMD_DFB_PASS=0, CMD_DFB_GAIN, CMD_DFB_DELAY};

    enum CmdLfoId {CMD_LFO_HZ=0, CMD_LFO_AMP, CMD_LFO_WAVE};

    enum CmdAnalogId {CMD_ANALOG_OUT=0};
    
    enum CmdHwInputId {CMD_HW_IN_LP_CUT=0, CMD_HW_IN_LP_RES, CMD_HW_IN_DFB_DELAY, CMD_HW_IN_DFB_GAIN, CMD_HW_IN_ANALOG_OUT};
    
    //fifth byte for OSC_WAVE
    enum OscWave {OSC_SQU=0, OSC_SAW, OSC_TRI, OSC_NOISE, OSC_DC, OSC_GROUP_SIZE};
    typedef enum OscWave OscWave;

    //fifth byte for OSC_WS
    enum WsType {WS_NONE=0, WS_AM, WS_HARDCLIP, WS_HARDCLIP_OFF, WS_TOPINVERT, WS_OVERDRIVE, WS_DESTROY, WS_GROUP_SIZE};
    typedef enum WsType WsType;
    
    enum AnalogOut {ANALOG_OUT_LP24=0, ANALOG_OUT_HP24, ANALOG_OUT_BP24, ANALOG_OUT_RAW, ANALOG_NUM_OUT};
    typedef enum AnalogOut AnalogOut;
    
    //fifth byte for SYS_INFO: firmware version
    //sixth byte for SYS_INFO: 123
    

    //ret/cmd must be (atleast) char[CMD_BYTES]
    void cmdMake(CmdTypeId type, CmdGroupId group, char param, char id, unsigned short data, unsigned char* ret);
    void cmdDecode(unsigned char* cmd, char* type, char* group, char* param, char* id, unsigned short* data);

#ifdef __cplusplus
}
#endif

#endif
