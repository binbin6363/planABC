package com.imo.msg;

/**
 * Created with IntelliJ IDEA.
 * User: bbwang
 * Date: 16-1-13
 * Time: 上午10:37
 * To change this template use File | Settings | File Templates.
 */
public interface Result {

    public boolean encode();

    public byte[] toBytes();
}
