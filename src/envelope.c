#include "envelope.h"

void envInit(Envelope* env)
{
    env->a=0;
    env->d=0;
    env->s=0;
    env->r=0;

    envSetADSR(env, 0xff,0x1ff,0x7fff,0xff);
    env->state = ENV_IDLE;
    env->val = 0;
}

void envSetADSR(Envelope* env, uint a, uint d, uint s, uint r)
{
    envSetA(env, a);
    envSetD(env, d);
    envSetS(env, s);
    envSetR(env, r);
}

void envSetA(Envelope* env, uint a)
{
    if (env->a == a)return;

    env->a = a;

    if(env->a == 0)env->dA = 0;
    else env->dA = 0xffff / env->a;
}

void envSetD(Envelope* env, uint d)
{
    if (env->d == d)return;

    env->d = d;

    if(env->d == 0)env->dD = 0;
    else env->dD =  0xffff / env->d;
}

void envSetS(Envelope* env, uint s)
{
    if (env->s == s)return;

    env->s = s;
    envSetD(env, env->d);
    envSetR(env, env->r);
}

void envSetR(Envelope* env, uint r)
{
    if (env->r == r)return;

    env->r = r;
    if(env->r == 0)env->dR = 0;
    else env->dR = 0xffff / env->r;
}

void envSetOn(Envelope* env, bool val)
{
    if (val) 
    {
        env->state = ENV_A;
    }
    else if(env->state != ENV_IDLE && env->state != ENV_BYPASS)
    {
        env->state = ENV_R;
    }
}

bool envGetOn(Envelope* env)
{
    if ( env->state  == ENV_IDLE || env->state == ENV_R || env->state == ENV_BYPASS)return false;
    return true;
}

void envSetBypass(Envelope* env)
{
    env->state = ENV_BYPASS;
}

void envBuffApply(Envelope* env, smp_t* buffer, uint n)
{
    if (env->state == ENV_BYPASS)return;

    uint i;
    //optimize in for loop
    for (i=0;i<n;i++)
    {
        if(env->state == ENV_A)
        {
            if(env->dA == 0)
            {
                env->val = 0xffff;
                env->state = ENV_D;
            }
            else
            {
                uint lastVal = env->val;
                env->val += env->dA;

                if(env->val == 0xffff || env->val < lastVal)
                {
                    env->val = 0xffff;
                    env->state = ENV_D;
                }
            }
        }
        if(env->state == ENV_D)
        {
            if(env->dD == 0)
            {
                env->val = env->s;
                env->state = ENV_S;
            }
            else
            {
                if(env->dD >= env->val)
                {
                    env->val = env->s;
                    env->state = ENV_S;
                }
                else
                {
                    env->val -= env->dD;
                    if(env->val <= env->s)
                    {
                        env->val = env->s;
                        env->state = ENV_S;
                    }
                }
            }
        }
        if(env->state == ENV_S)
        {
            //do nothing, if not env->s==0 which is special retrig case
            //this is OK because: when env->s is less than 255 it still counts
            //as zero volume: see buffer[i] = ...
            /*if(env->s == 0)
              {
              env->state = ENV_A;
              }*/
        }

        if(env->state == ENV_R)
        {
            if(env->dR == 0)
            {
                env->val = 0;
                env->state = ENV_IDLE;
            }
            else
            {
                if(env->val <=  env->dR)
                {
                    env->val = 0;
                    env->state = ENV_IDLE;
                }
                else env->val -= env->dR;
            }
        }
        buffer[i] = (buffer[i] * (env->val>>8))>>8;
    }
}



void envBuffGen(Envelope* env, smp_t* buffer, uint n, smp_t min, smp_t max)
{
    if (env->state == ENV_BYPASS)return;

    uint i;
    uint dAmp = max - min;
    //optimize in for loop
    for (i=0;i<n;i++)
    {
        if(env->state == ENV_A)
        {
            if(env->dA == 0)
            {
                env->val = 0xffff;
                env->state = ENV_D;
            }
            else
            {
                uint lastVal = env->val;
                env->val += env->dA;

                if(env->val == 0xffff || env->val < lastVal)
                {
                    env->val = 0xffff;
                    env->state = ENV_D;
                }
            }
        }
        if(env->state == ENV_D)
        {
            if(env->dD == 0)
            {
                env->val = env->s;
                env->state = ENV_S;
            }
            else
            {
                env->val -= env->dD;
                if(env->val <= env->s)
                {
                    env->val = env->s;
                    env->state = ENV_S;
                }
            }
        }
        if(env->state == ENV_S)
        {
            //do nothing
        }

        if(env->state == ENV_R)
        {
            if(env->dR == 0)
            {
                env->val = 0;
                env->state = ENV_IDLE;
            }
            else
            {
                if(env->val <=  env->dR)
                {
                    env->val = 0;
                    env->state = ENV_IDLE;
                }
                else env->val -= env->dR;
            }
        }
        buffer[i] = min + ((dAmp * (env->val>>8)) >> 8);
    }
}

void envADJumpValue(Envelope* env, uint* retval, uint steps, uint min, uint max)
{
    if (env->state == ENV_BYPASS)return;
    
    uint dAmp = max - min;
    
    if(env->state == ENV_A)
    {
        if(env->dA == 0)
        {
            env->val = 0xffff;
            env->state = ENV_D;
        }
        else
        {
            uint lastVal = env->val;
            env->val += env->dA * steps;

            if(env->val == 0xffff || env->val < lastVal)
            {
                env->val = 0xffff - env->val;
                env->state = ENV_D;
            }
        }
    }
    if(env->state == ENV_D)
    {
        if(env->dD == 0 || env->val == 0)
        {
            env->val = 0;
            env->state = ENV_IDLE;
        }
        else
        {
            uint lastVal = env->val;
            env->val -= env->dD * steps;
            
            if(env->val == 0 || env->val > lastVal)
            {
                env->val = 0;
                env->state = ENV_IDLE;
            }
        }
    }
    *retval = min + (dAmp>>8) * (env->val>>8);
}
