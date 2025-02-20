#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "check ls with a valid built-in argument runs without errors" {
    run ./dsh <<EOF
ls -l
EOF

    [ "$status" -eq 0 ]
}

@test "Handling command with only a single quote" {
    run ./dsh <<EOF
echo '
EOF

    [ "$status" -eq 0 ]
}

@test "no ending quote, program doesn't break" {
    run ./dsh <<EOF
echo "NO ending quote
EOF

    [ "$status" -eq 0 ]
}

@test "2 commands at the same time work" {
    run "./dsh" <<EOF
    echo first
    echo second
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="firstseconddsh2> dsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Long command with many values executes correctly" {
    run "./dsh" <<EOF
echo  wow hello   hey    there wowow

EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="wow hello hey there wowowdsh2> dsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Input with only spaces does nothing" {
    run ./dsh <<EOF
                           
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="dsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

# my first level test case of quoted and unquoted
@test "both arguments with quoted and non-quoted arguments works correctly" {
    run ./dsh <<EOF
    echo first "second third" fourth
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="first second third fourthdsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

# my second level test case of single and double quotes and no quotes
@test "both single and double quotes and no quote in one command works correctly" {
    run ./dsh <<EOF
echo 'single' "double" noquote

EOF
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="single double noquotedsh2> dsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "cd multiple times and changes directory correctly" {
    run ./dsh <<EOF
cd /tmp
mkdir -p testdir
cd testdir
pwd
cd ..
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')

    expected_output="/tmp/testdir/tmpdsh2>dsh2>dsh2>dsh2>dsh2>dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "echo command with backslash outputs correctly" {
    run ./dsh <<EOF
echo "back\\slash thing"

EOF
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    expected_output="back\slash thingdsh2> dsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}