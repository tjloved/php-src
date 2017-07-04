--TEST--
ZE2 type hinting
--FILE--
<?php

class P
{
}
class T
{
    function f(P $p = NULL)
    {
        var_dump($p);
        echo "-\n";
    }
}
function fn653419938()
{
    $o = new T();
    $o->f(new P());
    $o->f();
    $o->f(NULL);
}
fn653419938();
--EXPECT--
object(P)#2 (0) {
}
-
NULL
-
NULL
-

