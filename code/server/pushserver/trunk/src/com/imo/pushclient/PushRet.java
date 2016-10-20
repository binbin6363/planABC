package com.imo.pushclient;

public class PushRet
{
    private String status;
    private String resultID;
    private int    resultCode;

    public PushRet(){
        status = "ok";
        resultID = "";
        resultCode = 0;
    }

    public String getStatus()
    {
        return status;
    }
    
    public void setStatus(String status)
    {
        this.status = status;
    }
    
    public int getResultCode()
    {
        return resultCode;
    }
    
    public void setResultCode(int resultCode)
    {
        this.resultCode = resultCode;
    }
    
    public String getResultID()
    {
        return resultID;
    }
    
    public void setResultID(String resultID)
    {
        this.resultID = resultID;
    }

    @Override
    public String toString() {
        return  " push result, code:" + resultCode + "; ret status:" + status + "; result id:" + resultID;
    }
}
