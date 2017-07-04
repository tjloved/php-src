--TEST--
Exception after separation during indirect write to fcall result
--FILE--
<?php

function throwing()
{
    throw new Exception();
}
function getArray($x)
{
    return [$x];
}
function fn413619838()
{
    try {
        getArray(0)[throwing()] = 1;
    } catch (Exception $e) {
        echo "Exception\n";
    }
}
fn413619838();
--EXPECT--
Exception
