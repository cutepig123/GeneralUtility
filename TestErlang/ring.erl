-module(ring).
-export([main/1]).

setup(N) ->
    init(N-1, self()).

run(N, M, Pid) ->
    send_message(Pid, 0),
    Count = manager(M-1, Pid),
    Count = N * M.  % Check for correct number of messages

init(0, Pid) ->
    Pid;
init(N, Pid) ->
    init(N-1, spawn(fun() -> worker(Pid) end)).

manager(0, _Pid) ->
    receive
	Msg ->
	    Msg
    end;
manager(M, Pid) ->
    relay_message(Pid),
    manager(M-1, Pid).

worker(Pid) ->
    relay_message(Pid),
    worker(Pid).

send_message(Pid, Msg) ->
    Pid ! Msg+1.

relay_message(Pid) ->
    receive
	Msg ->
	    send_message(Pid, Msg)
    end.

get_times(Fun) ->
    statistics(runtime),
    statistics(wall_clock),
    Result = Fun(),
    {_, CPUTime} = statistics(runtime),
    {_, ElapsedTime} = statistics(wall_clock),
    {Result, CPUTime/1000, ElapsedTime/1000}.

main(Args) ->
    [N, M] = [list_to_integer(atom_to_list(S)) || S <- Args],
    {Pid, SetupCPU, SetupElapsed} = get_times(fun() -> setup(N) end),
    {_, RunCPU, RunElapsed} = get_times(fun() -> run(N, M, Pid) end),
    io:format("~p ~p ~p ~p ~p ~p~n",
	      [N, M, SetupCPU, SetupElapsed, RunCPU, RunElapsed]).
