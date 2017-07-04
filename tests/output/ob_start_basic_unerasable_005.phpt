--TEST--
ob_start(): Ensure unerasable buffer cannot be flushed by ob_flush()
--FILE--
<?php

function callback($string)
{
    static $callback_invocations;
    $callback_invocations++;
    return "[callback:{$callback_invocations}]{$string}\n";
}
function fn1822517353()
{
    ob_start('callback', 0, false);
    echo "Attempt to flush unerasable buffer - should fail... ";
    var_dump(ob_flush());
    // Check content of buffer after flush - if flush failed it should still contain the string above.
    var_dump(ob_get_contents());
}
fn1822517353();
--EXPECTF--
[callback:1]Attempt to flush unerasable buffer - should fail... 
Notice: ob_flush(): failed to flush buffer of callback (0) in %s on line %d
bool(false)
string(%d) "Attempt to flush unerasable buffer - should fail... 
Notice: ob_flush(): failed to flush buffer of callback (0) in %s on line %d
bool(false)
"
