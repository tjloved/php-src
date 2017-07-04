--TEST--
__DIR__ constant used with eval()
--FILE--
<?php

function fn1006297830()
{
    eval('echo __DIR__ . "\\n";');
}
fn1006297830();
--EXPECTF--
%stests%sconstants
