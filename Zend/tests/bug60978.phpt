--TEST--
Bug #60978 (exit code incorrect)
--SKIPIF--
<?php
	if ("cli" != php_sapi_name()) {
		echo "skip CLI only";
	}
?>
--FILE--
<?php

function fn1463362857()
{
    $php = getenv('TEST_PHP_EXECUTABLE');
    exec($php . ' -n -r "exit(2);"', $output, $exit_code);
    echo $exit_code;
}
fn1463362857();
--EXPECT--
2
