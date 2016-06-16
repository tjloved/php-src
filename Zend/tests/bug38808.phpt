--TEST--
Bug #38808 ("maybe ref" issue for current() and others)
--FILE--
<?php

function fn1686357939()
{
    $current = "current";
    $next = "next";
    $b = array(1 => 'one', 2 => 'two');
    $a =& $b;
    echo $current($a) . "\n";
    $next($a);
    echo $current($a) . "\n";
}
fn1686357939();
--EXPECT--
one
two