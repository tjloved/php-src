--TEST--
Standard behaviour of __DIR__
--FILE--
<?php

function fn461548352()
{
    echo __DIR__ . "\n";
    echo dirname(__FILE__) . "\n";
}
fn461548352();
--EXPECTF--
%stests%sconstants
%stests%sconstants
