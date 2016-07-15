check_tool()
{
	prefix=$1
	varname=$2
	tool=$3

	if ! [ -e "${prefix}/bin/${tool}" ]
	then
		echo "bin/${tool} does not exist in prefix ${prefix}"
		exit 1
	fi

	if ! [ -x "${prefix}/bin/${tool}" ]
	then
		echo "${prefix}/bin/${tool} is not executable"
		exit 1
	fi

	export X${varname}=${prefix}/bin/${tool}
}
