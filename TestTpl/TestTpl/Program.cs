using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Threading.Tasks.Dataflow;
using System.Threading.Tasks;
using System.Threading;
using System.Runtime.InteropServices;

namespace TPL.Test.DataFlows
{
    class Program
    {
        [DllImport("kernel32", EntryPoint = "GetCurrentThreadId")]
        static extern int GetCurrentThreadId();

        static void Main(string[] args)
        {
            var writeOut = new ActionBlock<string>(s =>
            {
                Console.WriteLine("Thread {0} Nid {2} s {1}", System.Threading.Thread.CurrentThread.ManagedThreadId, s, GetCurrentThreadId());
            }
            );
            var broadcast = new BroadcastBlock<string>(s => s.ToString());
            var transform = new TransformBlock<string, string>(s => s.ToUpper());
            var buffer = new BufferBlock<DateTime>();
            var join = new JoinBlock<string, DateTime>();
            var joinWrite = new ActionBlock<Tuple<string, DateTime>>(t => writeOut.Post(t.Item1 + " at " + t.Item2.ToString()));
            broadcast.LinkTo(transform);
            broadcast.LinkTo(writeOut);
            transform.LinkTo(join.Target1);
            buffer.LinkTo(join.Target2);
            join.LinkTo(joinWrite);

            writeOut.Post("test");
            Console.WriteLine("any key to continue...");
            Console.ReadKey();

            //Begin activating everything
            Task.Factory.StartNew(() =>
            {
                while (true)
                {
                    Thread.Sleep(2000);
                    buffer.Post(
                    DateTime.Now);
                }
            },
            TaskCreationOptions.LongRunning);
            Console.WriteLine("any key to continue...");
            Console.ReadKey();

            var itr = 0;
            while (itr < 5)
            {
                broadcast.Post(
                "New string " + Guid.NewGuid().ToString());
                Thread.Sleep(1000);
                ++itr;
            }
            Console.WriteLine("Execution complete, any key to continue...");
            Console.ReadKey();
        }
    }
}
