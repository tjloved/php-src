--TEST--
Bug #61681: Malformed grammar
--FILE--
<?php

function fn1910879257()
{
    $la = "ooxx";
    echo "{${substr('laruence', 0, 2)}}";
}
fn1910879257();
--EXPECT--
ooxx
