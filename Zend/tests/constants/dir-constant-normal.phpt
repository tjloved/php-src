--TEST--
Standard behaviour of __DIR__
--FILE--
<?php

function fn1362978624()
{
    echo __DIR__ . "\n";
    echo dirname(__FILE__) . "\n";
}
fn1362978624();
--EXPECTF--
%stests%sconstants
%stests%sconstants
