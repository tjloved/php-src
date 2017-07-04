--TEST--
Testing register_shutdown_function() with timeout. (Bug: #21513)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

function boo()
{
    echo "Shutdown\n";
}
function fn675506093()
{
    ini_set('display_errors', 0);
    echo "Start\n";
    register_shutdown_function("boo");
    /* not necessary, just to show the error sooner */
    set_time_limit(1);
    /* infinite loop to simulate long processing */
    for (;;) {
    }
    echo "End\n";
}
fn675506093();
--EXPECT--
Start
Shutdown
