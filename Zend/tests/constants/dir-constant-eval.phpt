--TEST--
__DIR__ constant used with eval()
--FILE--
<?php

function fn1942270578()
{
    eval('echo __DIR__ . "\\n";');
}
fn1942270578();
--EXPECTF--
%stests%sconstants
