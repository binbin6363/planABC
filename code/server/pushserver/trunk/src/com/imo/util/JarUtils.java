/**
 * @file JarUtils.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月26日-下午1:39
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-26	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.util;

import java.io.File;
import java.io.IOException;
import java.util.Map;
import java.util.jar.Attributes;
import java.util.jar.JarFile;
import java.util.jar.Manifest;

public class JarUtils {

    private static File getFile() {
        //关键是这行...
        String path = JarUtils.class .getProtectionDomain().getCodeSource().getLocation().getFile();
        try {
            path = java.net.URLDecoder.decode(path, "UTF-8" ); //转换处理中文及空格
        }catch  (java.io.UnsupportedEncodingException e){
            return null;
        }
        return new File(path);
    }

    //获取jar绝对路径
    public static String getJarPath(){
        File file = getFile();
        if (file == null ){
            return null ;
        }
        return  file.getAbsolutePath();
    }

    //获取jar目录
    public static String getJarDir() {
        File file = getFile();
        if (file == null) {
            return   null ;
        }
        return  getFile().getParent();
    }

    //获取jar包名
    public static String getJarName() {
        File file = getFile();
        if (file == null) {
            return   null ;
        }
        return  getFile().getName();
    }


    public static String getVersionInfo(String path){
        String jarPath;
        if (path == null) {
            jarPath = getJarPath();
        } else {
            jarPath = path;
        }
        StringBuilder stringBuilder = new StringBuilder();
        //stringBuilder.append("show version info:\n");
        JarFile jarfile = null;
        Manifest manifest = null;
        try {
            jarfile = new JarFile(jarPath);
            manifest = jarfile.getManifest();
        } catch (IOException e) {
            e.printStackTrace();
            return stringBuilder.toString();
        } finally {
            try {
                if (jarfile != null) {
                    jarfile.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        Attributes attributes = manifest.getMainAttributes();
        for (Map.Entry<Object, Object> entry : attributes.entrySet()) {
            stringBuilder.append(entry.getKey().toString());
            stringBuilder.append(":");
            stringBuilder.append(entry.getValue().toString());
            stringBuilder.append("\n");
        }
        return stringBuilder.toString();
    }


    public static void main(String[] args) throws java.io.IOException{
        String versionInfo = getVersionInfo("E:\\myspace\\eclipse_worlspace\\demo\\out\\artifacts\\pushserver\\pushserver.jar");
        System.out.println(versionInfo);
    }
} // class JarUtils end
