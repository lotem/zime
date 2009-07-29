package cn.zzsst.client;


import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public abstract class ZimeEngine {

	protected ZimeModule module;
	
	protected Schema schema;

    protected Dict dict;

    protected Context context;

    protected CandidateList candidateList;

	public ZimeEngine(ZimeModule module, Schema schema) {
		this.module = module;
		this.schema = schema;
		this.dict = schema.getDict();
		this.context = new Context();
		this.candidateList = new CandidateList();
	}
	
	public abstract boolean processKeyDownEvent(KeyDownEvent event);

	public abstract boolean processKeyUpEvent(KeyUpEvent event);
	
}
