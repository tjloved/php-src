--TEST--
Generators can also yield keys
--FILE--
<?php

function reverse(array $array)
{
    end($array);
    while (null !== ($key = key($array))) {
        (yield $key => current($array));
        prev($array);
    }
}
function fn260863235()
{
    $array = ['foo' => 'bar', 'bar' => 'foo'];
    foreach (reverse($array) as $key => $value) {
        echo $key, ' => ', $value, "\n";
    }
}
fn260863235();
--EXPECT--
bar => foo
foo => bar
