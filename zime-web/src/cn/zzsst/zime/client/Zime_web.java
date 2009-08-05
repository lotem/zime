package cn.zzsst.zime.client;

import com.google.gwt.core.client.EntryPoint;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Zime_web implements EntryPoint {

    @Override
    public void onModuleLoad() {
        // TODO: register components here.
        Registry.register(new StaticFileDictFactory());
        Registry.register(new RomanEngineFactory());
        
        Registry.load(new WebModule());
    }
    
}
