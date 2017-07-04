--TEST--
Bug #61681: Malformed grammar
--FILE--
<?php

function fn468254823()
{
    $la = "ooxx";
    echo "{${substr('laruence', 0, 2)}}";
}
fn468254823();
--EXPECT--
ooxx
