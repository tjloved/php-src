--TEST--
unset() CV 4 (unset() local variable in included file)
--FILE--
<?php

function f()
{
    $x = "ok\n";
    echo $x;
    include "unset.inc";
    echo $x;
}
function fn1833480497()
{
    f();
}
fn1833480497();
--EXPECTF--
ok

Notice: Undefined variable: x in %sunset_cv04.php on line %d
