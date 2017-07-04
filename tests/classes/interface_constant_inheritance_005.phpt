--TEST--
Ensure a interface can have public constants
--FILE--
<?php

interface IA
{
    public const FOO = 10;
}
function fn317798225()
{
    echo "Done\n";
}
fn317798225();
--EXPECT--
Done