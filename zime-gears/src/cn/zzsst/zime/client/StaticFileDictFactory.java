package cn.zzsst.zime.client;

public class StaticFileDictFactory implements DictFactory {

    @Override
    public Dict create(String codeName) {
        return new StaticFileDict(codeName);
    }

}
