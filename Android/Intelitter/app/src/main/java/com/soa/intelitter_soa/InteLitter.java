package com.soa.intelitter_soa;

public class InteLitter 
{
    // States values
    private final static int MIN_STATE_VALUE = 2000;
    private final static int MAX_STATE_VALUE = 2005;
    public final static int STATE_CLEAN = 2000;
    public final static int STATE_SLIGHTLY_DIRTY = 2001;
    public final static int STATE_MID_DIRTY = 2002;
    public final static int STATE_DIRTY = 2003;
    public final static int STATE_CLEANING = 2004;
    public final static int STATE_CAT_INSIDE = 2005;



    public static boolean isStateValue(int value) 
    {
        return value >= MIN_STATE_VALUE && value <= MAX_STATE_VALUE;
    }
}
