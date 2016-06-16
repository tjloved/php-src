--TEST--
isset() with object properties when operating on non-object
--FILE--
<?php

function fn364951575()
{
    $foo = NULL;
    isset($foo->bar->bar);
    echo "Done\n";
}
fn364951575();
--EXPECT--	
Done
