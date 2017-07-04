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
function fn852305928()
{
    $array = ['foo' => 'bar', 'bar' => 'foo'];
    foreach (reverse($array) as $key => $value) {
        echo $key, ' => ', $value, "\n";
    }
}
fn852305928();
--EXPECT--
bar => foo
foo => bar
