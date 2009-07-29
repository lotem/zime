package cn.zzsst.client;

import java.util.ArrayList;

public interface Dict {

    ArrayList<String> lookup(Context context);
    String parse(String preedit);
}
