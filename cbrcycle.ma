[top]
components : retrieve@Retrieve reuse@Reuse casebase@CaseBase retain@Retain revise@Revise 
out : solution
in : query
in : feedback
Link : query query@retrieve
Link : feedback feedback@revise
Link : retrieve@retrieve retrieve@casebase
Link : provide@casebase provide@retrieve
Link : proposed@retrieve proposed@reuse
Link : adapted@reuse adapted@revise
Link : fixed@revise in@retain
Link : out@retain store@casebase
Link : solution@reuse solution
Link : solution@revise solution


[retrieve]
distribution : normal
mean : 0.1
deviation : 0.01

[retain]
distribution : normal
mean : 5
deviation : 1

[casebase]
distribution : normal
mean : 1
deviation : 0.5

[revise]
distribution : normal
mean : 8
deviation : 2

[reuse]
distribution : normal
mean : 8
deviation : 2