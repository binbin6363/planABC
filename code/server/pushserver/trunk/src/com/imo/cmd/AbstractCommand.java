/**
 * @file AbstractCommand.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月12日-上午10:34
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-12	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.cmd;

import com.imo.msg.BinaryPacket;
import org.apache.mina.core.session.IoSession;

public class AbstractCommand implements BaseCommand {

    public AbstractCommand(BinaryPacket message, IoSession session){

    }

    @Override
    public boolean execute() {
        return false;
    }
} // class AbstractCommand end
