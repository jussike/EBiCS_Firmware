# Motor Control Analysis for Lishui FOC Firmware (Sensorless Focus)

## 1. Sensorless Motor Control Architecture

In this implementation, specifically for sensorless operation where Hall sensors are not available, the system relies on a **Flux-Based Observer** to estimate rotor position.

### Core Components:
- **Phase Current Sensing**: Phase currents are sampled via ADC using shunt resistors. In `main.c`, `HAL_ADCEx_InjectedConvCpltCallback` handles the reading. It includes **Dynamic ADC State** logic to select the best phases to sample based on the current PWM duty cycle (ensuring the low-side MOSFET is on long enough for a clean sample).
- **Flux-Based Observer (`observer_update` in `FOC.c`)**: This module implements a flux-based observer based on the Lee-Hong-Nam-Ortega paper. It uses the motor model (resistance, inductance, and flux linkage) and measured phase voltages/currents to estimate back-EMF components ($\alpha, \beta$).
- **Angle Estimation**: The estimated back-EMF components are converted into a rotor angle via `atan2_LUT`. This angle is critical for Park and Inverse Park transforms in sensorless mode.
- **Current Regulation**: The $i_q$ target is derived from throttle/PAS. PI controllers calculate $u_d$ and $u_q$ to drive the current errors to zero.

## 2. Parameters Required for a New Motor (Sensorless)

For reliable sensorless operation, the following motor parameters in `config.h` are mandatory:

- **Phase Resistance (`RESISTANCE`)**: Needed for the observer to calculate the resistive voltage drop ($I \cdot R$).
- **Phase Inductance (`INDUCTANCE`)**: Essential for the observer to model flux linkage changes.
- **Flux Linkage (`FLUX_LINKAGE`)**: Represents the rotor's magnetic strength. This is the main reference for the flux observer to determine position.
- **Observer Gain (`GAMMA`)**: Controls the convergence speed of the observer. Must be tuned for stability.
- **Pole Pairs**: Necessary for calculating mechanical speed and mechanical-to-electrical mapping.

## 3. Detailed Motor Parameter Measurement Procedure

To configure the firmware for a new motor without Hall sensors, perform the following measurements:

### 3.1 Phase Resistance ($R$)
1.  **Measurement**: Use a high-precision multimeter or a milliohm meter.
2.  **Procedure**: Measure the resistance between any two of the three motor phase wires ($R_{L-L}$).
3.  **Calculation**: For a Star (Y) connected motor, the phase resistance is **$R = R_{L-L} / 2$**.
4.  **Scaling**: Convert to the format expected in `config.h` (e.g., ensure units match the scaled `long long` values).

### 3.2 Phase Inductance ($L$)
1.  **Measurement**: Use an LCR meter.
2.  **Procedure**: Set the test frequency to 10kHz or 16kHz (matching the inverter's PWM frequency). Measure the inductance between two phase wires ($L_{L-L}$).
3.  **Calculation**: The phase inductance is **$L = L_{L-L} / 2$**.
4.  **Note**: Rotate the motor slowly while measuring; if the value fluctuates, use the average.

### 3.3 Flux Linkage ($\lambda$)
1.  **Measurement**: Spin the motor with an external driver (like a drill) and use an oscilloscope.
2.  **Procedure**:
    *   Spin the motor at a constant, known RPM (measure with a tachometer).
    *   Measure the peak-to-peak voltage ($V_{p-p}$) between two phase leads.
    *   **Calculations**:
        1. Calculate the Peak-to-Neutral voltage: $V_{pk\_phase} = (V_{p-p} / 2) / \sqrt{3}$.
        2. Calculate the electrical angular velocity: $\omega_e = (RPM / 60) \cdot 2\pi \cdot PolePairs$.
        3. The flux linkage is: **$\lambda = V_{pk\_phase} / \omega_e$**.
3.  **Result**: Adjust `FLUX_LINKAGE` in `config.h` based on this value.

### 3.4 Determining Pole Pairs
1.  **Procedure**: 
    *   Connect one probe of an oscilloscope between two motor phases.
    *   Rotate the motor shaft manually exactly one full mechanical revolution ($360^\circ$).
    *   Count the number of complete sine wave cycles generated.
2.  **Result**: The number of cycles equals the number of Pole Pairs.

### 3.5 Tuning `GAMMA` and `SPEC_ANGLE`
*   **`GAMMA`**: Start with a value like `13LL`. If the motor loses sync under load, try increasing it. If it vibrates or is noisy at high speed, try decreasing it.
*   **`SPEC_ANGLE`**: This defines the electrical offset between the observer's zero and the actual magnetic zero. Tune this by running the motor and adjusting until phase current is minimized for a given torque/speed.
