package zime.plume.client;

public interface Frontend {

    public abstract void updatePreedit(String preedit, int selStart, int selEnd);

    public abstract void setSelectionRange(int start, int length);

    public abstract void updateCandidates(CandidateList candidateList);

    public abstract void showCandidates();

    public abstract void hideCandidates();

    public abstract void commit(String text);

    public abstract void submit();

}
