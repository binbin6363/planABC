/**
 * @file HtmlPage.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月28日-下午12:25
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-28	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.util;

import org.apache.log4j.Logger;

import java.util.LinkedHashMap;
import java.util.Map;

public class HtmlPage {
    private static final Logger logger = Logger.getLogger(HtmlPage.class);
    public static final String PAGE_404 = "<html><body><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><h1>大写的悲伤，你懂的少年 ~_~ \n big sorrowful, you know why ~_~ </h1></body></html>";
    public static final String TAG_INDEX_PAGE = "index_page";
    public static final String TAG_404_PAGE = "404_page";
    public static final String TAG_RESULT_PAGE = "result_page";

    private static Map<String, String> pages = new LinkedHashMap<String, String>();

    static {
        pages.put(TAG_404_PAGE, PAGE_404);
    }

    public static boolean addPage(String pageName, String pageContent){
        pages.put(pageName, pageContent);
        return true;
    }

    public static String getPage(String name){
        if (!pages.containsKey(name)) {
            logger.warn("not found page:"+name);
            return pages.get(TAG_404_PAGE);
        }
        return pages.get(name);
    }

    public static void reset(){
        pages.clear();
    }

    public static boolean existPage(String page){
        return pages.containsKey(page);
    }
} // class HtmlPage end
