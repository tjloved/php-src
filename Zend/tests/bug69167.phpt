--TEST--
Bug #69167 (call_user_func does not support references anymore)
--FILE--
<?php

function l($m)
{
    echo $m . "\n";
}
function fn1110078607()
{
    $cb = 'l';
    call_user_func($cb, 'hi');
    $cb2 =& $cb;
    call_user_func($cb2, 'hi2');
}
fn1110078607();
--EXPECT--
hi
hi2
