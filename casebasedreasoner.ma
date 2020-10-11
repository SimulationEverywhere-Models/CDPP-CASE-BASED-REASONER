[top]
components : retrieve@Retrieve reuse@Reuse casebase@CaseBase retain@Retain revise@Revise interface@Interface
out : output
in : input
Link : query@interface query@retrieve
Link : feedback@interface feedback@revise
Link : retrieve@retrieve retrieve@casebase
Link : provide@casebase provide@retrieve
Link : proposed@retrieve proposed@reuse
Link : adapted@reuse adapted@revise
Link : fixed@revise in@retain
Link : out@retain store@casebase
Link : solution@reuse solution@interface
Link : solution@revise solution@interface
Link : input input@interface
Link : output@interface output


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

[interface]
distribution : normal
mean : 1
deviation : 0.5