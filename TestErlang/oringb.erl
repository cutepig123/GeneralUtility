% sets up a ring of N processes, a message is passed M times around the ring.
-module(oringb).
-export([run_benchmark/2, loop/2, mloop/1, main/1]).
% builds a ring of N processes and send a message around the ring M times.
run_benchmark(N, M) ->
io:format(">> Erlang R11B-5 here (N=~w, M=~w)!~n~n", [N, M]),
% spawn the other N-1 processes needed for the ring
LastP = lists:foldl(fun(S, Pid) -> spawn(?MODULE, loop, [N-S, Pid]) end,
self(), lists:seq(1, N-1)),
spawn(fun() -> [ LastP ! R || R <- lists:reverse(lists:seq(0, M-1)) ] end),
mloop(N).
% receive loop executed by the first N-1 processes in the ring
%   - 'S' is the sequence number allowing us to keep track of a
%     process' position in the ring
%   - 'NextP' is the next process in the ring
%   - 'R' is the round/message number, zero indicates that the processes
%      should terminate;
loop(S, NextP) ->
receive
% the message number is above zero => forward the message to the next
% process in the ring
R when R > 0 -> NextP ! R,
io:format(": Proc: ~8w, Seq#: ~w, Msg#: ~w ..~n", [self(), S, R]),
loop(S, NextP);
% the message number is zero => forward message and terminate
R when R =:= 0 -> NextP ! R,
io:format("* Proc: ~8w, Seq#: ~w, Msg#: terminate!~n", [self(), S]);
% error: the message number is below zero => raise exception
R when R < 0 ->
erlang:error({"internal error", "invalid message number"})
end.
% receive loop executed by the last (Nth) process;
% it won't forward any messages
mloop(S) ->
receive
R when R > 0 ->
io:format("> Proc: ~8w, Seq#: ~w, Msg#: ~w ..~n", [self(), S, R]),
mloop(S);
->
io:format("@ Proc: ~8w, Seq#: ~w, ring terminated.~n", [self(), S])
end.
% 'main' function allowing the invocation from the shell as well as the
% passing of command line arguments
main(A) ->
Args = [ list_to_integer(Litem) ||
Litem <- [ atom_to_list(Atom) || Atom <- A ]],
[N, M] = Args,
run_benchmark(N, M).
