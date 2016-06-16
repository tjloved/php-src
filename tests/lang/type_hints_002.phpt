--TEST--
ZE2 type hinting
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
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
function fn1838357108()
{
    $o = new T();
    $o->f(new P());
    $o->f();
    $o->f(NULL);
}
fn1838357108();
--EXPECT--
object(P)#2 (0) {
}
-
NULL
-
NULL
-

