Time=1208_25000126
in_dir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/Digi
out_dir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/Calib
run_dir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/Run
pedestal=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/pedestal.root
dac=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/dac.root
mip=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/mip.root

#pedestal=/cefs/higgs/shiyk/Beam_2023/DataBase/Pedestal/pedestal.root
#dac=/cefs/higgs/diaohb/CEPC2023/SPS/ratio/dac.root
#dac=/cefs/higgs/shiyk/Beam_2023/DataBase/Calib/DAC/Pion_Calib_2023_V2.root
#mip=/cefs/higgs/shiyk/Beam_2023/DataBase/Calib/MIP/all_V2_Fit.root
rm -rf ${run_dir}
mkdir -p ${out_dir}
mkdir -p ${run_dir}

function run()
{
        for element in `ls $in_dir$1`
        do  
                file=$in_dir$1"/"$element
                sub_dir=$1"/"$element
                if [ -d $file ]
                then
                        mkdir -p $out_dir$sub_dir
                        mkdir -p $run_dir$sub_dir
                        run $sub_dir
                else
                        hep_file=$run_dir$1"/"$element".run.sh"
                        out_file=$run_dir$1"/"$element".log"
                        out_rootfile=$out_dir$1"/"Calib_${element#*_}
                        echo -e \
                        "source ~/.bash_setup\nCalib $file $pedestal $dac $mip $out_rootfile" > ${hep_file}
                        chmod +x ${hep_file}
                        hep_sub ${hep_file} #-o ${out_file}
                fi  
        done
}

run

