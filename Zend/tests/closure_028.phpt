--TEST--
Closure 028: Trying to use lambda directly in foreach
--FILE--
<?php

function fn435128065()
{
    foreach (function () {
        return 1;
    } as $y) {
        var_dump($y);
    }
    print "ok\n";
}
fn435128065();
--EXPECT--
ok
