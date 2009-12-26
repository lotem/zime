package zime.plume.client;

public interface Frontend {

    void updatePreedit(String preedit, int selStart, int selEnd);

    void updateCandidates(CandidateList candidateList);

    void commit(String text);

    void submit();

}
