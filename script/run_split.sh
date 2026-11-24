#!/bin/bash
set -e
shopt -s failglob

Title=ML_0126_trigger60_model_25_material_0_e_gradient
Project=/cefs/higgs/diaohb/SIM/AHCAL-simulation
Dir=${Dir:-$Project/run/$Title}
RunDir=${WorkDir:-$Dir/Run_$(date +%Y%m%d_%H%M%S)}
ListDir=$Dir/list_SPS_nolow_correct
RunNumber=200
BatchSize=20

CalibDir=/cefs/higgs/diaohb/CEPC2023/SPS/calibration
pedestal=$CalibDir/pedestal.root
dac=$CalibDir/ratio/calib/pi-/calib.root
mip=$CalibDir/mip.root
spe=$CalibDir/spe.root
lowgain_adc=$CalibDir/lowgain_e-calib.root
sipm_model=/cefs/higgs/diaohb/SIM/saturation_model/sipm_model_0.0xt.root

samples=("$Dir"/run2/*.root)
setup=
if [[ -n ${EnvSetup:-} ]]; then printf -v setup 'source %q' "$EnvSetup"; fi
mkdir "$RunDir"
mkdir -p "$RunDir/jobs" "$RunDir/parts" "$ListDir"
for list in truth digi calib correct; do : > "$ListDir/$list"; done

# 1. 和原脚本一样，从 run2 取文件名，每个能量点单独处理。
for path in "${samples[@]}"; do
    datafile=${path##*/}
    base=${datafile%_*}
    tail=${base#*_}.root
    sample=$RunDir/parts/$base
    mkdir "$sample"

    calo=calo/$base.root
    mc=MC/MC_$tail
    digi=Digi_SPS_nolow/Digi_$tail
    calib=Calib_SPS_nolow/Calib_$tail
    correct=Correct_SPS_nolow/Correct_$tail
    echo "$Dir/$mc"      >> "$ListDir/truth"
    echo "$Dir/$digi"    >> "$ListDir/digi"
    echo "$Dir/$calib"   >> "$ListDir/calib"
    echo "$Dir/$correct" >> "$ListDir/correct"

    # 2. 每 20 个 run 写一个作业，里面直接放原来的五条处理命令。
    for ((first=1; first<=RunNumber; first+=BatchSize)); do
        last=$((first + BatchSize - 1))
        if ((last > RunNumber)); then last=$RunNumber; fi
        inputs=()
        for ((i=first; i<=last; i++)); do
            infile=$Dir/run$i/$datafile
            if [[ -f $infile ]]; then
                inputs+=("$infile")
            else
                echo "$infile" >> "$RunDir/missing_inputs"
            fi
        done
        (( ${#inputs[@]} )) || continue

        part=$sample/run_${first}_${last}
        mkdir "$part"
        printf '%s\n' "${inputs[@]}" > "$part/inputs.list"
        for file in "$calo" "$mc" "$digi" "$calib" "$correct"; do
            stage=${file%/*}
            mkdir -p "$part/$stage" "$Dir/$stage"
            echo "$part/$file" >> "$sample/$stage.list"
        done

        job=$RunDir/jobs/${base}_run${first}_${last}.sh
        cat > "$job" <<JOB
#!/bin/bash
set -e
cd "$part"
$setup
exec 8>.batch.lock
flock 8
if [[ ! -f DONE ]]; then
    rm -f "$calo" "$mc" "$digi" "$calib" "$correct"  # 补跑时清理本批未完成的输出
    hadd -f "$calo" @inputs.list
    root -l -b -q '$Project/script/BeamDataStructure.cxx("$calo","$mc",0)'
    digi "$mc" "$pedestal" "$dac" "$mip" "$spe" "$lowgain_adc" "$sipm_model" "$digi"
    Calib "$digi" "$pedestal" "$dac" "$mip" "$calib"
    satu_correct "$calib" "$pedestal" "$dac" "$mip" "$spe" "$correct"
    touch DONE
fi
cd "$sample"
flock "$Dir/$correct.lock" bash "$RunDir/merge.sh" "$Dir" "$calo" "$mc" "$digi" "$calib" "$correct"
JOB
        chmod +x "$job"
    done
done

# 3. 每批结束时调用这段脚本。等同一能量点的所有批次都完成，再按清单顺序合并。
cat > "$RunDir/merge.sh" <<'MERGE'
#!/bin/bash
set -e
Dir=$1
shift
for part in run_*/; do
    [[ -f $part/DONE ]] || exit 0
done
for file in "$@"; do
    stage=${file%/*}
    [[ -f $stage.merged ]] && continue
    hadd -f "$Dir/$file.tmp.root" "@$stage.list"
    mv -f "$Dir/$file.tmp.root" "$Dir/$file"
    touch "$stage.merged"
done
touch MERGED
MERGE

cat > "$ListDir/log" <<'LOG'
birks constant 0.126
original digitization
consider intercept in highgain lowgian ratio
add 0.015 temperature uniformity
increase highgain pedestal sigma to 2 times
increase lowgain pedestal sigma to 2 times
increase spe sigma to 8
add 0.02 non linearity (gaussian) to highgain lowgian adc
no noise
channel level noise: histogram sample
increase light yield uncertenty to 0.06
LOG

# 4. 所有作业和输入清单都已写好，逐个提交。
echo "作业脚本目录：$RunDir/jobs"
for job in "$RunDir"/jobs/*.sh; do
    hep_sub "$job" -o "$job.out" -e "$job.err"
done
