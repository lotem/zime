package cn.zzsst.client;

import java.util.ArrayList;

public interface ZimeDict {

    ArrayList<String> lookup(Context context);
    String parse(String preedit);
}
